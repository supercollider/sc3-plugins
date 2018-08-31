LADSPA : MultiOutUGen {
    classvar <>ladspalist_path;
    *ar {|nChans, id ... args|
        ^this.multiNewList(['audio',nChans,id] ++ args);
    }
    init {|... theInputs|
        inputs = theInputs;
        channels = {|n| OutputProxy(rate, this, n)} ! inputs[0];
        if (inputs[0] == 1, { ^channels[0] });
        ^channels
    }
    *initClass {
        ladspalist_path = LADSPA.filenameSymbol.asString.dirname+/+"ladspalist";
        ^super;
    }
    *listPlugins {
        ladspalist_path.quote.unixCmd;
    }
}

/*
perhaps we should interface with the ladspalist utility in a clever way? for example generate class wrappers for each ladspa plugin according? or some other way to make things easier than having to specify the unique ID?
then we could handle number of output-channels, default args and such..
Or at least create a GUI browser that can create a textstring with the arguments for a specific plugin, etc..
*/
