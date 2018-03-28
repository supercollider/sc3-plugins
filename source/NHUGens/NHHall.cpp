/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#include "nh_hall.hpp"
#include "SC_PlugIn.hpp"

static InterfaceTable* ft;

class SCAllocator {
public:
    World* m_world;
    SCAllocator(World* world) : m_world(world) { }

    void* ASSUME_ALIGNED(64) allocate(int memory_size) {
        return RTAlloc(m_world, memory_size);
    }

    void deallocate(void* memory) {
        RTFree(m_world, memory);
    }
};

class NHHall : public SCUnit {
public:
    NHHall() :
    m_core(
        sampleRate(),
        std::unique_ptr<SCAllocator>(new SCAllocator(mWorld))
    )
    {
        if (!m_core.m_initialization_was_successful) {
            printf("Could not allocate real-time memory for NHHall\n");
            set_calc_function<NHHall, &NHHall::clear>();
            return;
        }

        set_calc_function<NHHall, &NHHall::next>();

        m_last_k = m_core.compute_k_from_rt60(in0(2));
        m_last_stereo = in0(3);
        m_last_low_freq = in0(4);
        m_last_low_ratio = in0(5);
        m_last_hi_freq = in0(6);
        m_last_hi_ratio = in0(7);
        m_last_early_diffusion = in0(8);
        m_last_late_diffusion = in0(9);
        float mod_rate = in0(10);
        m_last_mod_depth = in0(11);

        m_core.m_k = m_last_k;
        m_core.set_stereo(m_last_stereo);
        m_core.set_low_shelf_parameters(m_last_low_freq, m_last_low_ratio);
        m_core.set_hi_shelf_parameters(m_last_hi_freq, m_last_hi_ratio);
        m_core.set_early_diffusion(m_last_early_diffusion);
        m_core.set_late_diffusion(m_last_late_diffusion);
        m_core.set_mod_rate(mod_rate);
        m_core.set_mod_depth(m_last_mod_depth);
    }

    template <typename UnitType, void (UnitType::*PointerToMember)(int)>
    void set_calc_function(void)
    {
        mCalcFunc = make_calc_function<UnitType, PointerToMember>();
        clear(1);
    }

private:
    nh_ugens::NHHall<SCAllocator> m_core;
    float m_last_k;
    float m_last_stereo;
    float m_last_low_freq;
    float m_last_low_ratio;
    float m_last_hi_freq;
    float m_last_hi_ratio;
    float m_last_early_diffusion;
    float m_last_late_diffusion;
    float m_last_mod_depth;

    void clear(int inNumSamples) {
        ClearUnitOutputs(this, inNumSamples);
    }

    void next(int inNumSamples) {
        const float* in_left = in(0);
        const float* in_right = in(1);
        const float rt60 = in0(2);
        float stereo = in0(3);
        float low_freq = in0(4);
        float low_ratio = in0(5);
        float hi_freq = in0(6);
        float hi_ratio = in0(7);
        float early_diffusion = in0(8);
        float late_diffusion = in0(9);
        float mod_rate = in0(10);
        float mod_depth = in0(11);

        float* out_left = out(0);
        float* out_right = out(1);

        float reciprocal_block_size = 1.0f / inNumSamples;

        float new_k = m_core.compute_k_from_rt60(rt60);
        float k = m_last_k;
        float k_ramp = (new_k - m_last_k) * reciprocal_block_size;

        // The stereo, low shelf, and hi shelf parameters are a bit expensive
        // to update, so I have opted to avoid recomputing them if they are
        // modulated.
        bool stereo_has_changed = stereo != m_last_stereo;
        bool low_shelf_parameters_have_changed = (
            low_freq != m_last_low_freq ||
            low_ratio != m_last_low_ratio
        );
        bool hi_shelf_parameters_have_changed = (
            hi_freq != m_last_hi_freq ||
            hi_ratio != m_last_hi_ratio
        );

        float stereo_ramp = (stereo - m_last_stereo) * reciprocal_block_size;
        float low_freq_ramp = (low_freq - m_last_low_freq) * reciprocal_block_size;
        float low_ratio_ramp = (low_ratio - m_last_low_ratio) * reciprocal_block_size;
        float hi_freq_ramp = (hi_freq - m_last_hi_freq) * reciprocal_block_size;
        float hi_ratio_ramp = (hi_ratio - m_last_hi_ratio) * reciprocal_block_size;
        float early_diffusion_ramp = (early_diffusion - m_last_early_diffusion) * reciprocal_block_size;
        float late_diffusion_ramp = (late_diffusion - m_last_late_diffusion) * reciprocal_block_size;
        float mod_depth_ramp = (mod_depth - m_last_mod_depth) * reciprocal_block_size;

        m_last_k = new_k;
        m_last_stereo = stereo;
        m_last_low_freq = low_freq;
        m_last_low_ratio = low_ratio;
        m_last_hi_freq = hi_freq;
        m_last_hi_ratio = hi_ratio;
        m_last_early_diffusion = early_diffusion;
        m_last_late_diffusion = late_diffusion;
        m_last_mod_depth = mod_depth;

        m_core.set_mod_rate(mod_rate);

        for (int i = 0; i < inNumSamples; i++) {
            m_core.m_k = k;
            k += k_ramp;

            if (stereo_has_changed) {
                m_core.set_stereo(stereo);
                stereo += stereo_ramp;
            }

            if (low_shelf_parameters_have_changed) {
                m_core.set_low_shelf_parameters(low_freq, low_ratio);
                low_freq += low_freq_ramp;
                low_ratio += low_ratio_ramp;
            }

            if (hi_shelf_parameters_have_changed) {
                m_core.set_hi_shelf_parameters(hi_freq, hi_ratio);
                hi_freq += hi_freq_ramp;
                hi_ratio += hi_ratio_ramp;
            }

            early_diffusion += early_diffusion_ramp;
            m_core.set_early_diffusion(early_diffusion);

            late_diffusion += late_diffusion_ramp;
            m_core.set_late_diffusion(late_diffusion);

            mod_depth += mod_depth_ramp;
            m_core.set_mod_depth(mod_depth);

            std::array<float, 2> result = m_core.process(in_left[i], in_right[i]);
            out_left[i] = result[0];
            out_right[i] = result[1];
        }
    }
};

PluginLoad(NHUGens) {
    ft = inTable;
    registerUnit<NHHall>(ft, "NHHall");
}
