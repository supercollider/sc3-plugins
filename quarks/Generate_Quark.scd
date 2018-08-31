// =====================================================================
// Generate Quark and overview helpfile
// nescivi, jan 2009
// =====================================================================

// this is a file to help automate creating Quarks of the plugins.


f = File.new( Document.current.dir.dirname +/+"VERSION", "r" );
~version = f.getLine;
f.close;
~version = ~version.asFloat

(
(Document.current.dir +/+ "build/*").pathMatch.do{ |it|
var name = PathName( it ).asRelativePath( Document.current.dir.dirname +/+ "build" ).copyFromStart( it.size - 1 );
	name.postln;
	f = File.new( (Document.current.dir.dirname +/+ "build/DIRECTORY" +/+ name ++".quark").postln, "w" );
	f.write( "(\n" );
	f.write( "\\name:\t\"" ++ name ++ "\",\n");
	f.write( "\\path:\t\"" ++ name ++ "\",\n");
	f.write( "\\summary:\t \"sc3-plugins:" ++ name ++ "\",\n");
	f.write( "\\author:\t \"sc3-plugins\",\n");
	f.write( "\\helpdoc:\t\"" ++ name ++ ".html\",\n");
	f.write( "\\url:\t \"http://sc3-plugins.sourceforge.net\",\n");
	f.write( "\\version:\t" ++ ~version ++ "\n" );
	f.write( ")\n" );
	f.close;
}
)



~sc3plugs = Quarks(
reposPath: "https://sc3-plugins.svn.sourceforge.net/svnroot/sc3-plugins/build",
localPath: Platform.userAppSupportDir +/+ "SC3plugins"
//localPath: "/home/nescivi/svn/sc3-plugins/build"
);

~sc3plugs.gui;

(
(Document.current.dir.dirname +/+ "build/*").pathMatch.do{ |it|
var name = PathName( it ).asRelativePath( Document.current.dir.dirname +/+ "build" ).copyFromStart( it.size - 1 );
	name.postln;
//var name = "NCAnalysisUGens";
q = ~sc3plugs.local.findQuark(name).info;

(
~header = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\"> \n
<html> \n
  <head> \n
    <title>"++ q.name ++ "</title> \n
  </head>\n
  <body>\n
  <h1>" ++ q.name ++ "</h1>\n
  <h2>" ++ q.summary ++ "</h2>\n
  <h3> by" + q.author ++ "</h3>\n
";
);


~footer = "</body></html>";

(
~string = "";
(~sc3plugs.local.path +/+ q.path +/+ "*.html").pathMatch.collect{ |it|
PathName( it ).asRelativePath( ~sc3plugs.local.path +/+ q.path  );
}.do{ |it|
	~string = ~string + "<A href=\""++it++"\">"++it++"<\A><br>\n";
};
(~sc3plugs.local.path +/+ q.path +/+ "help/*.*").pathMatch.collect{ |it|
PathName( it ).asRelativePath( ~sc3plugs.local.path +/+ q.path  );
}.do{ |it|
	~string = ~string + "<A href=\""++it++"\">"++it++"<\A><br>\n";
};
(~sc3plugs.local.path +/+ q.path +/+ "Help/*.*").pathMatch.collect{ |it|
PathName( it ).asRelativePath( ~sc3plugs.local.path +/+ q.path  );
}.do{ |it|
	~string = ~string + "<A href=\""++it++"\">"++it++"<\A><br>\n";
};
);


f = File.new( ~sc3plugs.local.path +/+ q.path +/+ q.name ++".html", "w" );
f.write( ~header );
f.write( ~string );
f.write( ~footer );

f.close;

};
);

// edit the quark file here to point to the right helpdoc
(Quarks.local.path +/+ "DIRECTORY" +/+ q.name ++".quark").openDocument;




(
~header = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\"> \n
<html> \n
  <head> \n
    <title>"++ q.name ++ "</title> \n
  </head>\n
  <body>\n
  <h1>" ++ q.name ++ "</h1>\n
  <h2>" ++ q.summary ++ "</h2>\n
  <h3> by" + q.author ++ "</h3>\n
";
);


~footer = "</body></html>";
