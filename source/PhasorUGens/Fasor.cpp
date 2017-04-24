/*
	SuperCollider real time audio synthesis system
 Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "SC_PlugIn.h"
static InterfaceTable *ft;

struct Fasor : public Unit {
    double mLevel;
    float m_previn;
    int ppdir;											//direction for pingpong
};

struct Fasor2 : public Unit {
    double mLevel;
    float m_previn;
    int ppdir;											//direction for pingpong
};

extern "C" {
    void load(InterfaceTable *inTable);
    void Fasor_Ctor(Fasor *unit);
    void Fasor_next_kk(Fasor *unit, int inNumSamples);
    void Fasor_next_ak(Fasor *unit, int inNumSamples);
    void Fasor_next_aa(Fasor *unit, int inNumSamples);
    void Fasor2_Ctor(Fasor2 *unit);
    void Fasor2_next_kk(Fasor2 *unit, int inNumSamples);
    void Fasor2_next_ak(Fasor2 *unit, int inNumSamples);
    void Fasor2_next_aa(Fasor2 *unit, int inNumSamples);
}

PluginLoad(InterfaceTable *inTable) {
    ft= inTable;
    DefineSimpleUnit(Fasor);
    DefineSimpleUnit(Fasor2);
}

void Fasor_Ctor(Fasor *unit) {
    if(unit->mCalcRate==calc_FullRate) {
        if(INRATE(0)==calc_FullRate) {
            if(INRATE(1)==calc_FullRate) {
                SETCALC(Fasor_next_aa);
            } else {
                SETCALC(Fasor_next_ak);
            }
        } else {
            SETCALC(Fasor_next_kk);
        }
    } else {
        SETCALC(Fasor_next_ak);
    }
    unit->m_previn= ZIN0(0);
    unit->ppdir= 1;
    ZOUT0(0)= unit->mLevel= ZIN0(2);
}

void Fasor_next_kk(Fasor *unit, int inNumSamples) {
    float *out= ZOUT(0);
    float in= ZIN0(0);
    float rate= sc_max(0, ZIN0(1));
    double start= ZIN0(2);
    double end= ZIN0(3);
    float loop= ZIN0(4);
    float previn= unit->m_previn;
    double level= unit->mLevel;
    
    if((previn<=0.f)&&(in>0.f)) {
        level= start;
    }
    
    if(loop<=0.f) {										//kk off
        if(end<start) {
            LOOP(inNumSamples,
                 ZXP(out)= level;
                 level-= rate;
                 level= sc_clip(level, end, start);
                 );
        } else {
            LOOP(inNumSamples,
                 ZXP(out)= level;
                 level+= rate;
                 level= sc_clip(level, start, end);
                 );
        }
        
    } else if(loop<=1.f) {								//kk forward
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        if(end<start) {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     ZXP(out)= level;
                     level-= rate;
                     if(level<loopstart) {
                         level= sc_wrap(level, loopend, loopstart);
                     }
                     );
            } else {
                LOOP(inNumSamples,
                     ZXP(out)= level;
                     if((level>=loopstart)&&(level<=loopend)) {
                         level+= rate;
                         if(level>loopend) {
                             level= loopstart;
                         }
                     } else {
                         level-= rate;
                         if(level<loopstart) {
                             level= loopend;
                         }
                     }
                     );
            }
        } else {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     ZXP(out)= level;
                     if((level>=loopend)&&(level<=loopstart)) {
                         level-= rate;
                         if(level<loopend) {
                             level= loopstart;
                         }
                     } else {
                         level+= rate;
                         if(level>loopstart) {
                             level= loopstart;
                         }
                     }
                     );
            } else {
                LOOP(inNumSamples,
                     ZXP(out)= level;
                     level+= rate;
                     if(level>loopend) {
                         level= sc_wrap(level, loopstart, loopend);
                     }
                     );
            }
        }
        
    } else /*if(loop<=2.f)*/ {							//kk pingpong
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        int ppdir= unit->ppdir;
        if(end<start) {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     ZXP(out)= level;
                     if((level>=loopend)&&(level<=loopstart)) {
                         level+= rate*ppdir;
                         if(level<loopend) {
                             level= loopend;
                             ppdir= 1;
                         } else if(level>loopstart) {
                             level= loopstart;
                             ppdir= -1;
                         }
                     } else {
                         level-= rate;
                         if(level<loopend) {
                             level= loopstart;
                         }
                     }
                     );
            } else {
                LOOP(inNumSamples,
                     ZXP(out)= level;
                     if((level>=loopstart)&&(level<=loopend)) {
                         level+= rate*ppdir;
                         if(level>loopend) {
                             level= loopend;
                             ppdir= -1;
                         } else if(level<loopstart) {
                             level= loopstart;
                             ppdir= 1;
                         }
                     } else {
                         level-= rate;
                         if(level<loopstart) {
                             level= loopend;
                         }
                     }
                     );
            }
        } else {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     ZXP(out)= level;
                     if((level>=loopend)&&(level<=loopstart)) {
                         level+= rate*ppdir;
                         if(level<loopend) {
                             level= loopend;
                             ppdir= 1;
                         } else if(level>loopstart) {
                             level= loopstart;
                             ppdir= -1;
                         }
                     } else {
                         level+= rate;
                         if(level>loopstart) {
                             level= loopstart;
                         }
                     }
                     );
            } else {
                LOOP(inNumSamples,
                     ZXP(out)= level;
                     if((level>=loopstart)&&(level<=loopend)) {
                         level+= rate*ppdir;
                         if(level>loopend) {
                             level= loopend;
                             ppdir= -1;
                         } else if(level<loopstart) {
                             level= loopstart;
                             ppdir= 1;
                         }
                     } else {
                         level+= rate;
                         if(level>loopend) {
                             level= loopstart;
                         }
                     }
                     );
            }
        }
        unit->ppdir= ppdir;
    }
    
    unit->m_previn= in;
    unit->mLevel= level;
}

void Fasor_next_ak(Fasor *unit, int inNumSamples) {
    float *out= ZOUT(0);
    float *in= ZIN(0);
    float rate= sc_max(0, ZIN0(1));
    double start= ZIN0(2);
    double end= ZIN0(3);
    float loop= ZIN0(4);
    float previn= unit->m_previn;
    double level= unit->mLevel;
    
    if(loop<=0.f) {										//ak off
        if(end<start) {
            LOOP(inNumSamples,
                 float curin= ZXP(in);
                 if((previn<=0.f)&&(curin>0.f)) {
                     float frac= 1.f-(previn/(curin-previn));
                     level= start+(frac*rate);
                 }
                 ZXP(out)= level;
                 level-= rate;
                 level= sc_clip(level, end, start);
                 previn= curin;
                 );
        } else {
            LOOP(inNumSamples,
                 float curin= ZXP(in);
                 if((previn<=0.f)&&(curin>0.f)) {
                     float frac= 1.f-(previn/(curin-previn));
                     level= start+(frac*rate);
                 }
                 ZXP(out)= level;
                 level+= rate;
                 level= sc_clip(level, start, end);
                 previn= curin;
                 );
        }
        
    } else if(loop<=1.f) {								//ak forward
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        if(end<start) {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*rate);
                     }
                     ZXP(out)= level;
                     level-= rate;
                     if(level<loopstart) {
                         level= sc_wrap(level, loopend, loopstart);
                     }
                     previn= curin;
                     );
            } else {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*rate);
                     }
                     ZXP(out)= level;
                     if((level>=loopstart)&&(level<=loopend)) {
                         level+= rate;
                         if(level>loopend) {
                             level= loopstart;
                         }
                     } else {
                         level-= rate;
                         if(level<loopstart) {
                             level= loopend;
                         }
                     }
                     previn= curin;
                     );
            }
        } else {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*rate);
                     }
                     ZXP(out)= level;
                     if((level>=loopend)&&(level<=loopstart)) {
                         level-= rate;
                         if(level<loopend) {
                             level= loopstart;
                         }
                     } else {
                         level+= rate;
                         if(level>loopstart) {
                             level= loopstart;
                         }
                     }
                     previn= curin;
                     );
            } else {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*rate);
                     }
                     ZXP(out)= level;
                     level+= rate;
                     if(level>loopend) {
                         level= sc_wrap(level, loopstart, loopend);
                     }
                     previn= curin;
                     );
            }
        }
        
    } else /*if(loop<=2.f)*/ {							//ak pingpong
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        int ppdir= unit->ppdir;
        if(end<start) {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*rate);
                         ppdir= -1;
                     }
                     ZXP(out)= level;
                     if((level>=loopend)&&(level<=loopstart)) {
                         level+= rate*ppdir;
                         if(level<loopend) {
                             level= loopend;
                             ppdir= 1;
                         } else if(level>loopstart) {
                             level= loopstart;
                             ppdir= -1;
                         }
                     } else {
                         level-= rate;
                         if(level<loopend) {
                             level= loopstart;
                         }
                     }
                     previn= curin;
                     );
            } else {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*rate);
                         ppdir= -1;
                     }
                     ZXP(out)= level;
                     if((level>=loopstart)&&(level<=loopend)) {
                         level+= rate*ppdir;
                         if(level>loopend) {
                             level= loopend;
                             ppdir= -1;
                         } else if(level<loopstart) {
                             level= loopstart;
                             ppdir= 1;
                         }
                     } else {
                         level-= rate;
                         if(level<loopstart) {
                             level= loopend;
                         }
                     }
                     previn= curin;
                     );
            }
        } else {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*rate);
                         ppdir= 1;
                     }
                     ZXP(out)= level;
                     if((level>=loopend)&&(level<=loopstart)) {
                         level+= rate*ppdir;
                         if(level<loopend) {
                             level= loopend;
                             ppdir= 1;
                         } else if(level>loopstart) {
                             level= loopstart;
                             ppdir= -1;
                         }
                     } else {
                         level+= rate;
                         if(level>loopstart) {
                             level= loopstart;
                         }
                     }
                     previn= curin;
                     );
            } else {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*rate);
                     }
                     ZXP(out)= level;
                     if((level>=loopstart)&&(level<=loopend)) {
                         level+= rate*ppdir;
                         if(level>loopend) {
                             level= loopend;
                             ppdir= -1;
                         } else if(level<loopstart) {
                             level= loopstart;
                             ppdir= 1;
                         }
                     } else {
                         level+= rate;
                         if(level>loopend) {
                             level= loopstart;
                         }
                     }
                     previn= curin;
                     );
            }
        }
        unit->ppdir= ppdir;
    }
    
    unit->m_previn= previn;
    unit->mLevel= level;
}

void Fasor_next_aa(Fasor *unit, int inNumSamples) {
    float *out= ZOUT(0);
    float *in= ZIN(0);
    float *rate= ZIN(1);
    double start= ZIN0(2);
    double end= ZIN0(3);
    float loop= ZIN0(4);
    float previn= unit->m_previn;
    double level= unit->mLevel;
    
    if(loop<=0.f) {										//aa off
        if(end<start) {
            LOOP(inNumSamples,
                 float curin= ZXP(in);
                 float zrate= ZXP(rate);
                 if((previn<=0.f)&&(curin>0.f)) {
                     float frac= 1.f-(previn/(curin-previn));
                     level= start+(frac*zrate);
                 }
                 ZXP(out)= level;
                 level-= zrate;
                 level= sc_clip(level, end, start);
                 previn= curin;
                 );
        } else {
            LOOP(inNumSamples,
                 float curin= ZXP(in);
                 float zrate= ZXP(rate);
                 if((previn<=0.f)&&(curin>0.f)) {
                     float frac= 1.f-(previn/(curin-previn));
                     level= start+(frac*zrate);
                 }
                 ZXP(out)= level;
                 level+= zrate;
                 level= sc_clip(level, start, end);
                 previn= curin;
                 );
        }
        
    } else if(loop<=1.f) {								//aa forward
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        if(end<start) {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     float zrate= ZXP(rate);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*zrate);
                     } 
                     ZXP(out)= level;
                     level-= zrate;
                     if(level<loopstart) {
                         level= sc_wrap(level, loopend, loopstart);
                     }
                     previn= curin;
                     );
            } else {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     float zrate= ZXP(rate);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*zrate);
                     } 
                     ZXP(out)= level;
                     if((level>=loopstart)&&(level<=loopend)) {
                         level+= zrate;
                         if(level>loopend) {
                             level= loopstart;
                         }
                     } else {
                         level-= zrate;
                         if(level<loopstart) {
                             level= loopend;
                         }
                     }
                     previn= curin;
                     );
            }
        } else {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     float zrate= ZXP(rate);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*zrate);
                     } 
                     ZXP(out)= level;
                     if((level>=loopend)&&(level<=loopstart)) {
                         level-= zrate;
                         if(level<loopend) {
                             level= loopstart;
                         }
                     } else {
                         level+= zrate;
                         if(level>loopstart) {
                             level= loopstart;
                         }
                     }
                     previn= curin;
                     );
            } else {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     float zrate= ZXP(rate);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*zrate);
                     } 
                     ZXP(out)= level;
                     level+= zrate;
                     if(level>loopend) {
                         level= sc_wrap(level, loopstart, loopend);
                     }
                     previn= curin;
                     );
            }
        }
        
    } else /*if(loop<=2.f)*/ {							//aa pingpong
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        int ppdir= unit->ppdir;
        if(end<start) {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     float zrate= ZXP(rate);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*zrate);
                         ppdir= -1;
                     } 
                     ZXP(out)= level;
                     if((level>=loopend)&&(level<=loopstart)) {
                         level+= zrate*ppdir;
                         if(level<loopend) {
                             level= loopend;
                             ppdir= 1;
                         } else if(level>loopstart) {
                             level= loopstart;
                             ppdir= -1;
                         }
                     } else {
                         level-= zrate;
                         if(level<loopend) {
                             level= loopstart;
                         }
                     }
                     previn= curin;
                     );
            } else {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     float zrate= ZXP(rate);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*zrate);
                         ppdir= -1;
                     } 
                     ZXP(out)= level;
                     if((level>=loopstart)&&(level<=loopend)) {
                         level+= zrate*ppdir;
                         if(level>loopend) {
                             level= loopend;
                             ppdir= -1;
                         } else if(level<loopstart) {
                             level= loopstart;
                             ppdir= 1;
                         }
                     } else {
                         level-= zrate;
                         if(level<loopstart) {
                             level= loopend;
                         }
                     }
                     previn= curin;
                     );
            }
        } else {
            if(loopend<loopstart) {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     float zrate= ZXP(rate);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*zrate);
                         ppdir= 1;
                     }
                     ZXP(out)= level;
                     if((level>=loopend)&&(level<=loopstart)) {
                         level+= zrate*ppdir;
                         if(level<loopend) {
                             level= loopend;
                             ppdir= 1;
                         } else if(level>loopstart) {
                             level= loopstart;
                             ppdir= -1;
                         }
                     } else {
                         level+= zrate;
                         if(level>loopstart) {
                             level= loopstart;
                         }
                     }
                     previn= curin;
                     );
            } else {
                LOOP(inNumSamples,
                     float curin= ZXP(in);
                     float zrate= ZXP(rate);
                     if((previn<=0.f)&&(curin>0.f)) {
                         float frac= 1.f-(previn/(curin-previn));
                         level= start+(frac*zrate);
                     }
                     ZXP(out)= level;
                     if((level>=loopstart)&&(level<=loopend)) {
                         level+= zrate*ppdir;
                         if(level>loopend) {
                             level= loopend;
                             ppdir= -1;
                         } else if(level<loopstart) {
                             level= loopstart;
                             ppdir= 1;
                         }
                     } else {
                         level+= zrate;
                         if(level>loopend) {
                             level= loopstart;
                         }
                     }
                     previn= curin;
                     );
            }
        }
        unit->ppdir= ppdir;
    }
    
    unit->m_previn= previn;
    unit->mLevel= level;
}

void Fasor2_Ctor(Fasor2 *unit) {
    if(unit->mCalcRate==calc_FullRate) {
        if(INRATE(0)==calc_FullRate) {
            if(INRATE(1)==calc_FullRate) {
                SETCALC(Fasor2_next_aa);
            } else {
                SETCALC(Fasor2_next_ak);
            }
        } else {
            SETCALC(Fasor2_next_kk);
        }
    } else {
        SETCALC(Fasor2_next_ak);
    }
    unit->m_previn= ZIN0(0);
    unit->ppdir= 1;
    ZOUT0(0)= unit->mLevel= ZIN0(2);
}

void Fasor2_next_kk(Fasor2 *unit, int inNumSamples) {
    float *out= ZOUT(0);
    float in= ZIN0(0);
    float rate= ZIN0(1);
    double start= ZIN0(2);
    double end= ZIN0(3);
    float loop= ZIN0(4);
    float previn= unit->m_previn;
    double level= unit->mLevel;
    double reset= start;
    
    if(end<start) {
        double endTemp= end;
        end= start;
        start= endTemp;
    }
    
    if((previn<=0.f)&&(in>0.f)) {
        level= reset;
    }
    
    if(loop<=0.f) {										//kk off
        LOOP(inNumSamples,
             ZXP(out)= level;
             level+= rate;
             level= sc_clip(level, start, end);
             );
        
    } else if(loop<=1.f) {								//kk forward
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        if(loopend<loopstart) {
            double loopendTemp= loopend;
            loopend= loopstart;
            loopstart= loopendTemp;
        }
        LOOP(inNumSamples,
             ZXP(out)= level;
             level+= rate;
             if((rate>=0.f)&&(level>loopend)) {
                 level= level-(loopend-loopstart);
             } else if((rate<0.f)&&(level<loopstart)) {
                 level= level+(loopend-loopstart);
             }
             level= sc_clip(level, start, end);
             );
        
    } else /*if(loop<=2.f)*/ {							//kk pingpong
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        int ppdir= unit->ppdir;
        if(loopend<loopstart) {
            double loopendTemp= loopend;
            loopend= loopstart;
            loopstart= loopendTemp;
        }
        LOOP(inNumSamples,
             ZXP(out)= level;
             if((level>=loopstart)&&(level<=loopend)) {
                 level+= rate*ppdir;
                 if(level>loopend) {
                     level= loopend;
                     ppdir= ppdir * -1;
                 } else if(level<loopstart) {
                     level= loopstart;
                     ppdir= ppdir * -1;
                 }
             } else {
                 level+= rate;
             }
             level= sc_clip(level, start, end);
             );
        unit->ppdir= ppdir;
    }
    
    unit->m_previn= in;
    unit->mLevel= level;
}

void Fasor2_next_ak(Fasor2 *unit, int inNumSamples) {
    float *out= ZOUT(0);
    float *in= ZIN(0);
    float rate= ZIN0(1);
    double start= ZIN0(2);
    double end= ZIN0(3);
    float loop= ZIN0(4);
    float previn= unit->m_previn;
    double level= unit->mLevel;
    double reset= start;
    
    if(end<start) {
        double endTemp= end;
        end= start;
        start= endTemp;
    }
    
    if(loop<=0.f) {										//ak off
        LOOP(inNumSamples,
             float curin= ZXP(in);
             if((previn<=0.f)&&(curin>0.f)) {
                 float frac= 1.f-(previn/(curin-previn));
                 level= reset+(frac*rate);
             }
             ZXP(out)= level;
             level+= rate;
             level= sc_clip(level, start, end);
             previn= curin;
             );
        
    } else if(loop<=1.f) {								//ak forward
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        if(loopend<loopstart) {
            double loopendTemp= loopend;
            loopend= loopstart;
            loopstart= loopendTemp;
        }
        LOOP(inNumSamples,
             float curin= ZXP(in);
             if((previn<=0.f)&&(curin>0.f)) {
                 float frac= 1.f-(previn/(curin-previn));
                 level= reset+(frac*rate);
             }
             ZXP(out)= level;
             level+= rate;
             if((rate>=0.f)&&(level>loopend)) {
                 level= level-(loopend-loopstart);
             } else if((rate<0.f)&&(level<loopstart)) {
                 level= level+(loopend-loopstart);
             }
             level= sc_clip(level, start, end);
             previn= curin;
             );
        
    } else /*if(loop<=2.f)*/ {							//ak pingpong
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        int ppdir= unit->ppdir;
        if(loopend<loopstart) {
            double loopendTemp= loopend;
            loopend= loopstart;
            loopstart= loopendTemp;
        }
        LOOP(inNumSamples,
             float curin= ZXP(in);
             if((previn<=0.f)&&(curin>0.f)) {
                 float frac= 1.f-(previn/(curin-previn));
                 level= reset+(frac*rate);
                 ppdir= -1;
             }
             ZXP(out)= level;
             if((level>=loopstart)&&(level<=loopend)) {
                 level+= rate*ppdir;
                 if(level>loopend) {
                     level= loopend;
                     ppdir= ppdir * -1;
                 } else if(level<loopstart) {
                     level= loopstart;
                     ppdir= ppdir * -1;
                 }
             } else {
                 level+= rate;
             }
             level= sc_clip(level, start, end);
             previn= curin;
             );
        unit->ppdir= ppdir;
    }
    
    unit->m_previn= previn;
    unit->mLevel= level;
}

void Fasor2_next_aa(Fasor2 *unit, int inNumSamples) {
    float *out= ZOUT(0);
    float *in= ZIN(0);
    float *rate= ZIN(1);
    double start= ZIN0(2);
    double end= ZIN0(3);
    float loop= ZIN0(4);
    float previn= unit->m_previn;
    double level= unit->mLevel;
    double reset= start;
    
    if(end<start) {
        double endTemp= end;
        end= start;
        start= endTemp;
    }
    
    if(loop<=0.f) {										//aa off
        LOOP(inNumSamples,
             float curin= ZXP(in);
             float zrate= ZXP(rate);
             if((previn<=0.f)&&(curin>0.f)) {
                 float frac= 1.f-(previn/(curin-previn));
                 level= reset+(frac*zrate);
             }
             ZXP(out)= level;
             level+= zrate;
             level= sc_clip(level, start, end);
             previn= curin;
             );
        
    } else if(loop<=1.f) {								//aa forward
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        if(loopend<loopstart) {
            double loopendTemp= loopend;
            loopend= loopstart;
            loopstart= loopendTemp;
        }
        LOOP(inNumSamples,
             float curin= ZXP(in);
             float zrate= ZXP(rate);
             if((previn<=0.f)&&(curin>0.f)) {
                 float frac= 1.f-(previn/(curin-previn));
                 level= reset+(frac*zrate);
             }
             ZXP(out)= level;
             level+= zrate;
             if((zrate>=0.f)&&(level>loopend)) {
                 level= level-(loopend-loopstart);
             } else if((zrate<0.f)&&(level<loopstart)) {
                 level= level+(loopend-loopstart);
             }
             level= sc_clip(level, start, end);
             previn= curin;
             );
        
    } else /*if(loop<=2.f)*/ {							//aa pingpong
        double loopstart= ZIN0(5);
        double loopend= ZIN0(6);
        int ppdir= unit->ppdir;
        if(loopend<loopstart) {
            double loopendTemp= loopend;
            loopend= loopstart;
            loopstart= loopendTemp;
        }
        LOOP(inNumSamples,
             float curin= ZXP(in);
             float zrate= ZXP(rate);
             if((previn<=0.f)&&(curin>0.f)) {
                 float frac= 1.f-(previn/(curin-previn));
                 level= reset+(frac*zrate);
                 ppdir= -1;
             }
             ZXP(out)= level;
             if((level>=loopstart)&&(level<=loopend)) {
                 level+= zrate*ppdir;
                 if(level>loopend) {
                     level= loopend;
                     ppdir= ppdir * -1;
                 } else if(level<loopstart) {
                     level= loopstart;
                     ppdir= ppdir * -1;
                 }
             } else {
                 level+= zrate;
             }
             level= sc_clip(level, start, end);
             previn= curin;
             );
        unit->ppdir= ppdir;
    }
    
    unit->m_previn= previn;
    unit->mLevel= level;
}
