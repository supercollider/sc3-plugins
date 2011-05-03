/// Installing sc3-plugins quarks

// If you installed the sc3-plugins to a user location:
~sc3plugs = Quarks(
reposPath: "https://sc3-plugins.svn.sourceforge.net/svnroot/sc3-plugins/build",
localPath: Platform.userAppSupportDir +/+ "SC3plugins"
);

// If the sc3-plugins were installed to a system wide location:
~sc3plugs = Quarks(
reposPath: "https://sc3-plugins.svn.sourceforge.net/svnroot/sc3-plugins/build",
localPath: Platform.systemAppSupportDir +/+ "SC3plugins"
);


// Get a GUI of the available Quarks and install the ones you want to try out:
~sc3plugs.gui;


// For more general Help on Quarks, see the Quarks helpfile