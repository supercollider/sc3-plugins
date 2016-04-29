#include "SC_PlugIn.h"


// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

struct StatsTriggeredUnit : public Unit
{
};

struct MeanTriggered : public StatsTriggeredUnit
{
	float* m_circbuf;
	int m_circbufpos;
	float m_mean;
	int m_length;
};
struct MedianTriggered : public StatsTriggeredUnit
{
	float* m_circbuf;
	float* m_sortbuf;
	int m_circbufpos;
	int m_length;
	bool m_length_is_odd;
	int m_medianpos;
	float m_outval;
};

// declare unit generator functions
extern "C"
{
	void MeanTriggered_Ctor(MeanTriggered* unit);
	void MeanTriggered_next_xa(MeanTriggered *unit, int inNumSamples);
	void MeanTriggered_next_xk(MeanTriggered *unit, int inNumSamples);
	void MeanTriggered_Dtor(MeanTriggered* unit);

	void MedianTriggered_Ctor(MedianTriggered* unit);
	void MedianTriggered_next_xa(MedianTriggered *unit, int inNumSamples);
	void MedianTriggered_next_xk(MedianTriggered *unit, int inNumSamples);
	void MedianTriggered_SelectionSort(float *array, int length);
	void MedianTriggered_Dtor(MedianTriggered* unit);
};


//////////////////////////////////////////////////////////////////
void MeanTriggered_Ctor(MeanTriggered* unit)
{
	if (INRATE(1) == calc_FullRate) {
		SETCALC(MeanTriggered_next_xa);
	}else{
		SETCALC(MeanTriggered_next_xk);
	}

	int length = (int)ZIN0(2); // Fixed number of items to average over

	unit->m_circbuf = (float*)RTAlloc(unit->mWorld, length * sizeof(float));
	for(int i=0; i<length; i++){
		unit->m_circbuf[i] = 0.f;
	}
	unit->m_circbufpos = 0;
	unit->m_length = length;
	unit->m_mean = 0.f;

	// prime the pumps
	MeanTriggered_next_xk(unit, 1);
}


void MeanTriggered_next_xa(MeanTriggered* unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float *trig = IN(1);

	// Get state and instance variables from the struct
	float* circbuf = unit->m_circbuf;
	int circbufpos = unit->m_circbufpos;
	int length = unit->m_length;

	// This may or may not be recalculated as we go through the loop, depending on triggering
	float mean = unit->m_mean;
	float curr;

	int i, j;
	for(i=0; i<inNumSamples; ++i){
		if(*(trig++) > 0.f){
			curr = in[i];
			//printf("%g, ", curr);
			circbuf[circbufpos] = curr;
			circbufpos++;
			if(circbufpos==length){
				circbufpos = 0;
			}
			double total = 0.;
			for(j=0; j<length; ++j)
				total += circbuf[j];
			mean = (float)(total / length);
		}

		*(out++) = mean;
	}

	// Store state variables back
	unit->m_circbufpos = circbufpos;
	unit->m_mean = mean;
}
void MeanTriggered_next_xk(MeanTriggered* unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float trig = IN0(1);

	// Get state and instance variables from the struct
	float* circbuf = unit->m_circbuf;
	int circbufpos = unit->m_circbufpos;
	int length = unit->m_length;

	// This may or may not be recalculated as we go through the loop, depending on triggering
	float mean = unit->m_mean;
	float curr;

	int i, j;
	for(i=0; i<inNumSamples; ++i){
		if(trig > 0.f){
			curr = in[i];
			//printf("%g, ", curr);
			circbuf[circbufpos] = curr;
			circbufpos++;
			if(circbufpos==length){
				circbufpos = 0;
			}
			double total = 0.;
			for(j=0; j<length; ++j)
				total += circbuf[j];
			mean = (float)(total / length);
		}

		*(out++) = mean;
	}

	// Store state variables back
	unit->m_circbufpos = circbufpos;
	unit->m_mean = mean;
}
void MeanTriggered_Dtor(MeanTriggered* unit)
{
	//printf("\n");
	RTFree(unit->mWorld, unit->m_circbuf);
}


//////////////////////////////////////////////////////////////////


void MedianTriggered_Ctor(MedianTriggered* unit)
{
	if (INRATE(1) == calc_FullRate) {
		SETCALC(MedianTriggered_next_xa);
	}else{
		SETCALC(MedianTriggered_next_xk);
	}

	int length = (int)ZIN0(2); // Fixed number of items to average over

	unit->m_circbuf = (float*)RTAlloc(unit->mWorld, length * sizeof(float));
	unit->m_sortbuf = (float*)RTAlloc(unit->mWorld, length * sizeof(float));
	for(int i=0; i<length; i++){
		unit->m_circbuf[i] = 0.f;
	}
	unit->m_circbufpos = 0;
	unit->m_length = length;
	unit->m_outval = 0.f;
	unit->m_length_is_odd = (length % 2) == 1;
	// If odd, this number is the index to take. If even, we take this number and the one above.
	unit->m_medianpos = unit->m_length_is_odd ? ((length-1)/2) : (length/2 - 1);

	// prime the pumps
	MedianTriggered_next_xk(unit, 1);
}


void MedianTriggered_next_xa(MedianTriggered* unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float *trig = IN(1);

	// Get state and instance variables from the struct
	float* circbuf = unit->m_circbuf;
	float* sortbuf = unit->m_sortbuf;
	int circbufpos = unit->m_circbufpos;
	int length = unit->m_length;
	bool length_is_odd = unit->m_length_is_odd;
	int medianpos = unit->m_medianpos;

	// This may or may not be recalculated as we go through the loop, depending on triggering
	float median = unit->m_outval;

	float curr;

	int i;
	for(i=0; i<inNumSamples; ++i){
		if(*(trig++) > 0.f){
			curr = in[i];
			circbuf[circbufpos] = curr;
			circbufpos++;
			if(circbufpos==length){
				circbufpos = 0;
			}

			// NOW CALCULATE THE MEDIAN

			// Copy the data into the buffer for sorting
			// TODO: Implement the copy more efficiently using memcpy
			for(int i=0; i<length; ++i){
				sortbuf[i] = circbuf[i];
			}

			// Then sort
			MedianTriggered_SelectionSort(sortbuf, length);

			// Then update the median
			median = length_is_odd ? sortbuf[medianpos] : ((sortbuf[medianpos] + sortbuf[medianpos+1]) * 0.5f);
		}

		*(out++) = median;
	}

	// Store state variables back
	unit->m_circbufpos = circbufpos;
	unit->m_outval = median;
}
void MedianTriggered_next_xk(MedianTriggered* unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float trig = IN0(1);

	// Get state and instance variables from the struct
	float* circbuf = unit->m_circbuf;
	float* sortbuf = unit->m_sortbuf;
	int circbufpos = unit->m_circbufpos;
	int length = unit->m_length;
	bool length_is_odd = unit->m_length_is_odd;
	int medianpos = unit->m_medianpos;

	// This may or may not be recalculated as we go through the loop, depending on triggering
	float median = unit->m_outval;

	float curr;

	int i;
	for(i=0; i<inNumSamples; ++i){
		if(trig > 0.f){
			curr = in[i];
			circbuf[circbufpos] = curr;
			circbufpos++;
			if(circbufpos==length){
				circbufpos = 0;
			}

			// NOW CALCULATE THE MEDIAN

			// Copy the data into the buffer for sorting
			// TODO: Implement the copy more efficiently using memcpy
			for(int i=0; i<length; ++i){
				sortbuf[i] = circbuf[i];
			}

			// Then sort
			MedianTriggered_SelectionSort(sortbuf, length);

			// Then update the median
			median = length_is_odd ? sortbuf[medianpos] : ((sortbuf[medianpos] + sortbuf[medianpos+1]) * 0.5f);
		}

		*(out++) = median;
	}

	// Store state variables back
	unit->m_circbufpos = circbufpos;
	unit->m_outval = median;
}
void MedianTriggered_Dtor(MedianTriggered* unit)
{
	RTFree(unit->mWorld, unit->m_circbuf);
	RTFree(unit->mWorld, unit->m_sortbuf);
}




void MedianTriggered_SelectionSort(float *array, int length)
{
  // Algo is from http://en.wikibooks.org/wiki/Algorithm_implementation/Sorting/Selection_sort

  // Debug:
  /*
  int lllll = length;
  Print("\nBefore selectionsort: ");
  for(int j=0; j<lllll; j++){
	Print("%g, ", array[j]);
  }
  */

  int max, i;
  float temp;
  while(length > 0)
  {
    max = 0;
    for(i = 1; i < length; i++)
      if(array[i] > array[max])
        max = i;
    temp = array[length-1];
    array[length-1] = array[max];
    array[max] = temp;
    length--;
  }

  // Debug:
  /*
  Print("\nAfter selectionsort: ");
  for(int j=0; j<lllll; j++){
	Print("%g, ", array[j]);
  }
  */
}

//////////////////////////////////////////////////////////////////



// the load function is called by the host when the plug-in is loaded
PluginLoad(MCLDTriggeredStats)
{
	ft = inTable;

	DefineDtorUnit(MeanTriggered);
	DefineDtorUnit(MedianTriggered);
}
