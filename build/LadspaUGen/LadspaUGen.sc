LADSPA : MultiOutUGen {
    *ar { 
        arg ... args;
        ^this.multiNewList(['audio'] ++ args);
    }
    init { arg ... theInputs;
        inputs = theInputs;
        channels = {|n| OutputProxy(rate, this, n)} ! inputs[0];
        if (inputs[0] == 1, { ^channels[0] });
        ^channels
    }
}

/*
perhaps we should make a C-program that generates class wrappers for each ladspa plugin according to metadata?
then we could handle number of output-channels, default args and such..
and/or interface sclang to such a C-program to fill a table which are used at runtime to create instances
*/
