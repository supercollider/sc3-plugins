/*
NHHall -- a stereo reverb
Version 2018-06-24

https://github.com/snappizz/nh-ugens

Copyright 2018 Nathan Ho.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------------------------------------------------------

NHHall is an open source algorithmic reverb unit in a single C++11 header
file. Features:

- Allpass loop topology with delay line modulaton for a lush 90's IDM sound
- True stereo signal path with controllable spread
- Infinite hold support
- Respectable CPU use
- Sample-rate independence
- No dependencies outside the C++ standard library
- Bring your own real-time safe memory allocator (no unwanted malloc calls!)
- Permissive MIT license
- Clean, readable source code for easy modification

NHHall was designed by Nathan Ho specifically for SuperCollider, ChucK, and
Auraglyph. Special thanks:

- Spencer Salazar for advising the project
- Brian Heim and Scott Carver for C++ help
- Patrick Dupuis for testing and feedback

-------------------------------------------------------------------------------

USAGE:

NHHall provides only the wet signal. Pre-delay and post-EQ are left up to you.

If you don't mind a real-time-unsafe call to malloc when initializing the unit
(maybe you're in an NRT context, or you're an absolute rascal), instantiate
the NHHall template with no arguments:

    #include "nh_hall.hpp"

    // Setup:
    float sample_rate = 48000.0f;
    nh_ugens::NHHall<> nh_hall(sample_rate);

    if (!nh_hall.m_initialization_was_successful) {
        // This happens when a memory allocation failed.
        ...
    }

    // Calculation:
    float in_left = 0.0f;
    float in_right = 0.0f;
    float out_left, out_right;
    std::array<float, 2> result = nh_hall.process(in_left, in_right);
    out_left = result[0];
    out_right = result[1];

This uses a default allocator class nh_ugens::Allocator which simply wraps
malloc and free.

You can replace this with your own allocator class that defines "allocate" and
"deallocate" methods like this:

    class MyAllocator {
    public:
        Engine* m_engine;
        MyAllocator(Engine* engine) : m_engine(engine) { }

        void* allocate(int memory_size) {
            return m_engine->allocate(memory_size);
        }

        void deallocate(void* memory) {
            m_engine->free(memory_size);
        }
    };

Then instantiate a std::unique_ptr to a MyAllocator and pass it into the new
NHHall as a second argument:

    nh_ugens::NHHall<MyAllocator> nh_hall(
        sample_rate,
        std::unique_ptr<MyAllocator>(new MyAllocator(engine))
    );

The std::unique_ptr is passed into the NHHall with move semantics. I'm sorry if
that's a little complicated, but it ensures that the MyAllocator isn't
unceremoniously tampered with after NHHall receives it and makes the program
easier to reason about. After this initialization, use of the unit is unchanged
from the above.

If the allocator ever returns a null pointer during initialization, NHHall
immediately stops all further allocation and sets the
m_initialization_was_successful flag to false. You should always check that
flag and make sure it worked. If you forget to do this, running NHHall.process
will access garbage memory and probably crash your app.

The following settings are available:

    NHHall.set_rt60(float rt60)
        Set the 60 dB decay time for mid frequencies.

    NHHall.set_stereo(float stereo)
        Set the stereo spread. 0 keeps the two channel paths separate, 1 causes
        them to bleed into each other almost instantly.

    NHHall.set_low_shelf_parameters(float frequency, float ratio)
    NHHall.set_hi_shelf_parameters(float frequency, float ratio)
        Set the frequency cutoffs and decay ratios of the damping filters.

    NHHall.set_early_diffusion(float diffusion)
    NHHall.set_late_diffusion(float diffusion)
        Diffusion coefficients.

    NHHall.set_mod_rate(float mod_rate)
    NHHall.set_mod_depth(float mod_depth)
        Rate and depth of LFO. These are arbitrarily scaled so that 0..1 offers
        musically useful ranges.

    NHHall.seed(uint32_t seed)
        Seed the random LFO. By default, the LFO has a fixed seed.

Instead of using set_rt60, you can also use the utility function

    float NHHall.compute_k_from_rt60(float rt60)

and manually set the public member NHHall.m_k. When interpolating the reverb
time, it's better to linearly interpolate m_k than to interpolate rt60, because
m_k may be 1 for an infinite hold reverb, putting rt60 at infinity. Trying to
interpolate from a finite value to infinity is asking for trouble.

Speaking of interpolation, it's your responsibility to smooth out these
parameters to avoid clicks when modulating them. All parameters are worth
smoothing out except mod_rate, which only checks its frequency parameter
occasionally.

*/

#pragma once
#include <cstdlib> // malloc / free
#include <cstring> // memset
#include <memory> // std::unique_ptr
#include <array> // std::array
#include <cmath> // cosf/sinf

namespace nh_ugens {

typedef std::array<float, 2> Stereo;

static inline float flush_denormals(float x) {
    x += 1.0e-25;
    x -= 1.0e-25;
    return x;
}

static inline Stereo flush_denormals(Stereo x) {
    Stereo result = {{flush_denormals(x[0]), flush_denormals(x[1])}};
    return result;
}

static inline int next_power_of_two(int x) {
    int result = 1;
    while (result < x) {
        result *= 2;
    }
    return result;
}

static float interpolate_cubic(float x, float y0, float y1, float y2, float y3) {
    float c0 = y1;
    float c1 = y2 - 1 / 3.0 * y0 - 1 / 2.0 * y1 - 1 / 6.0 * y3;
    float c2 = 1 / 2.0 * (y0 + y2) - y1;
    float c3 = 1 / 6.0 * (y3 - y0) + 1 / 2.0 * (y1 - y2);
    return ((c3 * x + c2) * x + c1) * x + c0;
}

// Unitary rotation matrix.
static inline Stereo rotate(Stereo x, float cos, float sin) {
    Stereo result = {
        cos * x[0] - sin * x[1],
        sin * x[0] + cos * x[1]
    };
    return result;
}

constexpr float twopi = 6.283185307179586f;

// Default allocator -- not real-time safe!
class Allocator {
public:
    void* allocate(int memory_size) {
        return malloc(memory_size);
    }

    void deallocate(void* memory) {
        free(memory);
    }
};

// Quadrature sine LFO, not used.
class SineLFO {
public:
    SineLFO(
        float sample_rate
    ) :
    m_sample_rate(sample_rate),
    m_cosine(1.0f),
    m_sine(0.0f)
    {
    }

    void set_frequency(float frequency) {
        m_k = twopi * frequency / m_sample_rate;
    }

    Stereo process(void) {
        m_cosine -= m_k * m_sine;
        m_sine += m_k * m_cosine;
        Stereo out = {{m_cosine, m_sine}};
        return out;
    }

private:
    const float m_sample_rate;
    float m_k;
    float m_cosine;
    float m_sine;
};

class RandomLFO {
public:
    RandomLFO(
        float sample_rate
    ) :
    m_sample_rate(sample_rate)
    {
        update_amplitude();
    }

    inline void seed(uint32_t seed) {
        m_lcg_state = seed;
    }

    inline uint16_t run_lcg(void) {
        m_lcg_state = m_lcg_state * 22695477 + 1;
        uint16_t result = m_lcg_state >> 16;
        return result;
    }

    void set_rate(float rate) {
        rate = std::max(rate, 0.0f);
        m_frequency =
            k_min_frequency
            + rate * (k_max_frequency - k_min_frequency);
        update_amplitude();
    }

    void set_depth(float depth) {
        depth = std::max(std::min(depth, 1.0f), 0.0f);
        m_depth = depth;
        update_amplitude();
    }

    Stereo process(void) {
        if (m_timeout <= 0) {
            m_timeout = run_lcg() * 0.1f / m_frequency * m_sample_rate / 48000.0f;
            m_increment = (run_lcg() * (1.0f / 32767.0f) - 0.5f) * m_frequency / m_sample_rate;
        }
        m_timeout -= 1;
        m_phase += m_increment;
        // TODO: optimize
        Stereo result = {{
            sinf(m_phase) * m_amplitude,
            cosf(m_phase) * m_amplitude
        }};
        return result;
    }

private:
    const float m_sample_rate;
    uint32_t m_lcg_state = 1;
    int m_timeout = 0;

    float m_depth = 0.5f;

    float m_increment = 0.f;
    float m_phase = 0.f;
    float m_frequency = 10.f;
    float m_amplitude;

    static constexpr float k_min_frequency = 1.0f;
    static constexpr float k_max_frequency = 50.0f;
    static constexpr float k_max_depth = 5e-3f;

    void update_amplitude(void) {
        m_amplitude = m_depth * k_max_depth / m_frequency;
    }

public:
    static constexpr float k_max_amplitude = k_max_depth / k_min_frequency;
};

class DCBlocker {
public:
    DCBlocker(
        float sample_rate
    ) :
    m_sample_rate(sample_rate)
    {
    }

    float process(float in) {
        float x = in;
        float y = x - m_x1 + m_k * m_y1;
        float out = y;
        m_x1 = x;
        m_y1 = y;
        return out;
    }

private:
    const float m_sample_rate;

    float m_x1 = 0.0f;
    float m_y1 = 0.0f;
    float m_k = 0.99f;
};

class HiShelf {
public:
    HiShelf(
        float sample_rate
    ) :
    m_sample_rate(sample_rate)
    {
    }

    void set_parameters(float frequency, float ratio) {
        float w0 = twopi * frequency / m_sample_rate;
        float sin_w0 = sinf(w0);
        float cos_w0 = cosf(w0);
        float a = sqrtf(ratio);
        float s = 1.0f;
        float alpha = sin_w0 * 0.5 * sqrtf((a + 1 / a) * (1 / s - 1) + 2);
        float x = 2 * sqrtf(a) * alpha;
        float a0 = (a + 1) - (a - 1) * cos_w0 + x;
        float inv_a0 = 1 / a0;
        m_b0 = (a * ((a + 1) + (a - 1) * cos_w0 + x)) * inv_a0;
        m_b1 = (-2 * a * ((a - 1) + (a + 1) * cos_w0)) * inv_a0;
        m_b2 = (a * ((a + 1) + (a - 1) * cos_w0 - x)) * inv_a0;
        m_a1 = (2 * ((a - 1) - (a + 1) * cos_w0)) * inv_a0;
        m_a2 = ((a + 1) - (a - 1) * cos_w0 - x) * inv_a0;
    }

    float process(float in) {
        float out =
            m_b0 * in + m_b1 * m_x1 + m_b2 * m_x2
            - m_a1 * m_y1 - m_a2 * m_y2;
        m_x2 = m_x1;
        m_x1 = in;
        m_y2 = flush_denormals(m_y1);
        m_y1 = flush_denormals(out);
        return out;
    }

private:
    const float m_sample_rate;
    float m_x1 = 0.0f;
    float m_x2 = 0.0f;
    float m_y1 = 0.0f;
    float m_y2 = 0.0f;
    float m_b0 = 1.0f, m_b1 = 0.0f, m_b2 = 0.0f, m_a1 = 0.0f, m_a2 = 0.0f;
};

class LowShelf {
public:
    LowShelf(
        float sample_rate
    ) :
    m_sample_rate(sample_rate)
    {
    }

    void set_parameters(float frequency, float ratio) {
        float w0 = twopi * frequency / m_sample_rate;
        float sin_w0 = sinf(w0);
        float cos_w0 = cosf(w0);
        float a = sqrtf(ratio);
        float s = 1.0f;
        float alpha = sin_w0 * 0.5 * sqrtf((a + 1 / a) * (1 / s - 1) + 2);
        float x = 2 * sqrtf(a) * alpha;
        float a0 = (a + 1) + (a - 1) * cos_w0 + x;
        float inv_a0 = 1 / a0;
        m_b0 = (a * ((a + 1) - (a - 1) * cos_w0 + x)) * inv_a0;
        m_b1 = (2 * a * ((a - 1) - (a + 1) * cos_w0)) * inv_a0;
        m_b2 = (a * ((a + 1) - (a - 1) * cos_w0 - x)) * inv_a0;
        m_a1 = (-2 * ((a - 1) + (a + 1) * cos_w0)) * inv_a0;
        m_a2 = ((a + 1) + (a - 1) * cos_w0 - x) * inv_a0;
    }

    float process(float in) {
        float out =
            m_b0 * in + m_b1 * m_x1 + m_b2 * m_x2
            - m_a1 * m_y1 - m_a2 * m_y2;
        m_x2 = m_x1;
        m_x1 = in;
        m_y2 = flush_denormals(m_y1);
        m_y1 = flush_denormals(out);
        return out;
    }

private:
    const float m_sample_rate;
    float m_x1 = 0.0f;
    float m_x2 = 0.0f;
    float m_y1 = 0.0f;
    float m_y2 = 0.0f;
    float m_b0 = 1.0f, m_b1 = 0.0f, m_b2 = 0.0f, m_a1 = 0.0f, m_a2 = 0.0f;
};


class BaseDelay {
public:
    int m_size;
    float* m_buffer = nullptr;

    BaseDelay(
        float sample_rate,
        float max_delay,
        float delay
    ) :
    m_sample_rate(sample_rate)
    {
        int max_delay_in_samples = m_sample_rate * max_delay;
        m_size = next_power_of_two(max_delay_in_samples);
        m_mask = m_size - 1;

        m_read_position = 0;

        m_delay = delay;
        m_delay_in_samples = m_sample_rate * delay;
    }

protected:
    const float m_sample_rate;
    int m_mask;
    int m_read_position;
    float m_delay;
    int m_delay_in_samples;
};

// Fixed delay line.
class Delay : public BaseDelay {
public:
    Delay(
        float sample_rate,
        float delay
    ) :
    BaseDelay(sample_rate, delay, delay)
    {
    }

    float process(float in) {
        float out_value = m_buffer[(m_read_position - m_delay_in_samples) & m_mask];
        m_buffer[m_read_position] = in;
        m_read_position = (m_read_position + 1) & m_mask;
        float out = out_value;
        return out;
    }

    float tap(float delay) {
        int delay_in_samples = delay * m_sample_rate;
        int position = m_read_position - 1 - delay_in_samples;
        float out = m_buffer[position & m_mask];
        return out;
    }
};

// Fixed Schroeder allpass.
class Allpass : public BaseDelay {
public:
    float m_k = 0.5;

    Allpass(
        float sample_rate,
        float delay,
        float diffusion_sign
    ) :
    BaseDelay(sample_rate, delay, delay),
    m_diffusion_sign(diffusion_sign)
    {
    }

    void set_diffusion(float diffusion) {
        m_k = diffusion * m_diffusion_sign;
    }

    float process(float in) {
        float delayed_signal = m_buffer[(m_read_position - m_delay_in_samples) & m_mask];
        float feedback_plus_input = in + delayed_signal * m_k;
        m_buffer[m_read_position] = flush_denormals(feedback_plus_input);
        m_read_position = (m_read_position + 1) & m_mask;
        float out = feedback_plus_input * -m_k + delayed_signal;
        return out;
    }

private:
    float m_diffusion_sign;
};

// Schroeder allpass with variable delay and cubic interpolation.
class VariableAllpass : public BaseDelay {
public:
    float m_k = 0.5;

    VariableAllpass(
        float sample_rate,
        float delay,
        float max_mod_depth,
        float diffusion_sign
    ) :
    BaseDelay(sample_rate, delay + max_mod_depth + 4.0 / sample_rate, delay),
    m_diffusion_sign(diffusion_sign)
    {
    }

    void set_diffusion(float diffusion) {
        m_k = diffusion * m_diffusion_sign;
    }

    float process(float in, float offset) {
        float position = m_read_position - (m_delay + offset) * m_sample_rate;

        // This catches a very sneaky bug -- casting position to int rounds
        // toward zero. To mitigate this, we ensure that the position is always
        // above zero before rounding it down, using the fact that
        // (m_delay + offset) * m_sample_rate < m_size.
        position += m_size;

        int iposition = position;
        float position_frac = position - iposition;

        float y0 = m_buffer[iposition & m_mask];
        float y1 = m_buffer[(iposition + 1) & m_mask];
        float y2 = m_buffer[(iposition + 2) & m_mask];
        float y3 = m_buffer[(iposition + 3) & m_mask];

        float delayed_signal = interpolate_cubic(position_frac, y0, y1, y2, y3);

        float feedback_plus_input = in + delayed_signal * m_k;
        m_buffer[m_read_position] = flush_denormals(feedback_plus_input);
        m_read_position = (m_read_position + 1) & m_mask;
        float out = feedback_plus_input * -m_k + delayed_signal;

        return out;
    }

private:
    float m_diffusion_sign;
};

template <class Alloc = Allocator>
class NHHall {
public:
    float m_k;
    bool m_initialization_was_successful;

    NHHall(
        float sample_rate,
        std::unique_ptr<Alloc> allocator
    ) :
    m_sample_rate(sample_rate),
    m_allocator(std::move(allocator)),

    m_lfo(sample_rate),
    m_dc_blocker(sample_rate),

    m_low_shelves {{sample_rate, sample_rate, sample_rate, sample_rate}},
    m_hi_shelves {{sample_rate, sample_rate, sample_rate, sample_rate}},

    m_early_allpasses {{
        Allpass(sample_rate, 9.5e-3f, 1),
        Allpass(sample_rate, 12.0e-3f, -1),
        Allpass(sample_rate, 7.8e-3f, 1),
        Allpass(sample_rate, 14.2e-3f, -1),
        Allpass(sample_rate, 23.5e-3f, 1),
        Allpass(sample_rate, 8.0e-3f, -1),
        Allpass(sample_rate, 25.8e-3f, 1),
        Allpass(sample_rate, 7.2e-3f, -1)
    }},

    m_early_delays {{
        Delay(sample_rate, 5.45e-3),
        Delay(sample_rate, 3.25e-3),
        Delay(sample_rate, 2.36e-3),
        Delay(sample_rate, 7.17e-3)
    }},

    m_late_variable_allpasses {{
        VariableAllpass(sample_rate, 25.6e-3f, RandomLFO::k_max_amplitude, 1),
        VariableAllpass(sample_rate, 50.7e-3f, RandomLFO::k_max_amplitude, -1),
        VariableAllpass(sample_rate, 68.6e-3f, RandomLFO::k_max_amplitude, 1),
        VariableAllpass(sample_rate, 45.7e-3f, RandomLFO::k_max_amplitude, -1)
    }},

    m_late_allpasses {{
        Allpass(sample_rate, 41.4e-3f, -1),
        Allpass(sample_rate, 25.6e-3f, 1),
        Allpass(sample_rate, 29.4e-3f, -1),
        Allpass(sample_rate, 23.6e-3f, 1)
    }},

    m_late_delays {{
        Delay(sample_rate, k_delay_time_1),
        Delay(sample_rate, k_delay_time_2),
        Delay(sample_rate, k_delay_time_3),
        Delay(sample_rate, k_delay_time_4)
    }}

    {
        m_k = 0.0f;

        m_initialization_was_successful = allocate_delay_lines();
    }

    // If no allocator object is passed in, we try to make one ourselves by
    // calling the constructor with no arguments.
    NHHall(
        float sample_rate
    ) :
    NHHall(sample_rate, std::unique_ptr<Alloc>(new Alloc()))
    { }

    ~NHHall() {
        free_delay_lines();
    }

    inline float compute_k_from_rt60(float rt60) {
        return powf(0.001f, k_average_delay_time / rt60);
    }

    inline void set_rt60(float rt60) {
        m_k = compute_k_from_rt60(rt60);
    }

    inline void set_stereo(float stereo) {
        float angle = stereo * twopi * 0.25f;
        m_rotate_cos = cosf(angle);
        m_rotate_sin = sinf(angle);
    }

    inline void set_low_shelf_parameters(float frequency, float ratio) {
        float k = powf(m_k, 1.0f / ratio - 1.0f);
        k = std::max(k, 0.01f);
        for (auto& x : m_low_shelves) {
            x.set_parameters(frequency, k);
        }
    }

    inline void set_hi_shelf_parameters(float frequency, float ratio) {
        float k = powf(m_k, 1.0f / ratio - 1.0f);
        k = std::max(k, 0.01f);
        for (auto& x : m_hi_shelves) {
            x.set_parameters(frequency, k);
        }
    }

    inline void set_early_diffusion(float diffusion) {
        for (auto& x : m_early_allpasses) {
            x.set_diffusion(diffusion);
        }
    }

    inline void set_late_diffusion(float diffusion) {
        for (auto& x : m_late_allpasses) {
            x.set_diffusion(diffusion);
        }
        for (auto& x : m_late_variable_allpasses) {
            x.set_diffusion(diffusion);
        }
    }

    inline void set_mod_rate(float mod_rate) {
        m_lfo.set_rate(mod_rate);
    }

    inline void set_mod_depth(float mod_depth) {
        m_lfo.set_depth(mod_depth);
    }

    inline void seed(uint32_t seed) {
        m_lfo.seed(seed);
    }

    Stereo process(Stereo in) {
        Stereo lfo = m_lfo.process();

        Stereo early = process_early(in);

        Stereo out = process_outputs(early);

        Stereo late = {{
            process_late_left(early[0], lfo),
            process_late_right(early[1], lfo)
        }};
        late = rotate(late, m_rotate_cos, m_rotate_sin);
        m_feedback = flush_denormals(late);

        return out;
    }

    Stereo process(float in_left, float in_right) {
        Stereo in = {{in_left, in_right}};
        return process(in);
    }

private:
    static constexpr float k_delay_time_1 = 153.6e-3f;
    static constexpr float k_delay_time_2 = 94.3e-3f;
    static constexpr float k_delay_time_3 = 187.6e-3f;
    static constexpr float k_delay_time_4 = 123.6e-3f;

    static constexpr float k_average_delay_time =
        (k_delay_time_1 + k_delay_time_2 + k_delay_time_3 + k_delay_time_4) / 4.0f;

    std::unique_ptr<Alloc> m_allocator;

    const float m_sample_rate;

    Stereo m_feedback = {{0.f, 0.f}};

    float m_rotate_cos = 0.0f;
    float m_rotate_sin = 1.0f;

    RandomLFO m_lfo;
    DCBlocker m_dc_blocker;

    std::array<LowShelf, 4> m_low_shelves;
    std::array<HiShelf, 4> m_hi_shelves;

    // NOTE: When adding new delay units, don't forget to allocate the memory
    // in the constructor and free it in the destructor.
    std::array<Allpass, 8> m_early_allpasses;
    std::array<Delay, 4> m_early_delays;

    std::array<VariableAllpass, 4> m_late_variable_allpasses;
    std::array<Allpass, 4> m_late_allpasses;
    std::array<Delay, 4> m_late_delays;

    bool allocate_delay_lines() {
        for (auto& x : m_early_allpasses) {
            bool success = allocate_delay_line(x);
            if (!success) {
                return false;
            }
        }
        for (auto& x : m_early_delays) {
            bool success = allocate_delay_line(x);
            if (!success) {
                return false;
            }
        }
        for (auto& x : m_late_variable_allpasses) {
            bool success = allocate_delay_line(x);
            if (!success) {
                return false;
            }
        }
        for (auto& x : m_late_allpasses) {
            bool success = allocate_delay_line(x);
            if (!success) {
                return false;
            }
        }
        for (auto& x : m_late_delays) {
            bool success = allocate_delay_line(x);
            if (!success) {
                return false;
            }
        }
        return true;
    }

    void free_delay_lines() {
        for (auto& x : m_early_allpasses) {
            free_delay_line(x);
        }
        for (auto& x : m_early_delays) {
            free_delay_line(x);
        }
        for (auto& x : m_late_variable_allpasses) {
            free_delay_line(x);
        }
        for (auto& x : m_late_allpasses) {
            free_delay_line(x);
        }
        for (auto& x : m_late_delays) {
            free_delay_line(x);
        }
    }

    bool allocate_delay_line(BaseDelay& delay) {
        void* memory = m_allocator->allocate(sizeof(float) * delay.m_size);
        if (!memory) {
            return false;
        }
        delay.m_buffer = static_cast<float*>(memory);
        memset(delay.m_buffer, 0, sizeof(float) * delay.m_size);
        return true;
    }

    void free_delay_line(BaseDelay& delay) {
        if (delay.m_buffer != nullptr) {
            m_allocator->deallocate(delay.m_buffer);
        }
    }

    inline Stereo process_early(Stereo in) {
        Stereo sig = {{in[0], in[1]}};

        sig[0] = m_early_allpasses[0].process(sig[0]);
        sig[0] = m_early_allpasses[1].process(sig[0]);
        sig[1] = m_early_allpasses[2].process(sig[1]);
        sig[1] = m_early_allpasses[3].process(sig[1]);
        sig = rotate(sig, m_rotate_cos, m_rotate_sin);
        Stereo early = sig;

        sig[0] = m_early_delays[0].process(sig[0]);
        sig[1] = m_early_delays[1].process(sig[1]);

        sig[0] = m_early_allpasses[4].process(sig[0]);
        sig[0] = m_early_allpasses[5].process(sig[0]);
        sig[1] = m_early_allpasses[6].process(sig[1]);
        sig[1] = m_early_allpasses[7].process(sig[1]);
        sig = rotate(sig, m_rotate_cos, m_rotate_sin);
        early[0] += sig[0] * 0.5f;
        early[1] += sig[1] * 0.5f;

        return early;
    }

    inline float process_late_left(float early_left, Stereo lfo) {
        float sig = 0.f;

        sig += m_feedback[0];

        sig += early_left;
        sig = m_late_variable_allpasses[0].process(sig, -lfo[0]);
        sig = m_late_allpasses[0].process(sig);
        sig *= m_k;
        sig = m_late_delays[0].process(sig);
        sig = m_low_shelves[0].process(sig);
        sig = m_hi_shelves[0].process(sig);

        sig += early_left;
        sig = m_late_variable_allpasses[1].process(sig, -lfo[1]);
        sig = m_late_allpasses[1].process(sig);
        sig *= m_k;
        sig = m_late_delays[1].process(sig);
        sig = m_low_shelves[1].process(sig);
        sig = m_hi_shelves[1].process(sig);

        return sig;
    }

    inline float process_late_right(float early_right, Stereo lfo) {
        float sig = 0.f;

        sig += m_feedback[1];

        sig += early_right;
        sig = m_late_variable_allpasses[2].process(sig, lfo[0]);
        sig = m_late_allpasses[2].process(sig);
        sig *= m_k;
        sig = m_late_delays[2].process(sig);
        sig = m_low_shelves[2].process(sig);
        sig = m_hi_shelves[2].process(sig);

        sig += early_right;
        sig = m_late_variable_allpasses[3].process(sig, lfo[1]);
        sig = m_late_allpasses[3].process(sig);
        sig *= m_k;
        sig = m_late_delays[3].process(sig);
        sig = m_low_shelves[3].process(sig);
        sig = m_hi_shelves[3].process(sig);

        return sig;
    }

    inline Stereo process_outputs(Stereo early) {
        // Keep the inter-channel delays somewhere between 0.1 and 0.7 ms --
        // this allows the Haas effect to come in.

        Stereo out = {{early[0] * 0.5f, early[1] * 0.5f}};

        float haas_multiplier = -0.6f;

        out[0] += m_late_delays[0].tap(0.0e-3f);
        out[1] += m_late_delays[0].tap(0.3e-3f) * haas_multiplier;

        out[0] += m_late_delays[1].tap(0.0e-3f);
        out[1] += m_late_delays[1].tap(0.1e-3f) * haas_multiplier;

        out[0] += m_late_delays[2].tap(0.7e-3f) * haas_multiplier;
        out[1] += m_late_delays[2].tap(0.0e-3f);

        out[0] += m_late_delays[3].tap(0.2e-3f) * haas_multiplier;
        out[1] += m_late_delays[3].tap(0.0e-3f);

        return out;
    }
};

} // namespace nh_ugens
