+ SequenceableCollection {

	// array[row][col]
	*newFromFile { arg path, columnSeparator=$ , rowSeparator=$\n;
		var obj, file, char, line;

		file = File.new(path, "r");
		if( file.isOpen == false, { ^nil	});

		obj = this.new;
		char = file.getChar;

		while({ char.notNil },{

			line = "";

			// skip past tabs and spaces at start of new line
			while({(char == $ ) || (char == $\t)}, {
				char = file.getChar;
			});

			// read a line in
			while({ (char!=rowSeparator) && (char!=$\r) && char.notNil }, {
				line = line ++ char;
				char = file.getChar;
			});

			char = file.getChar;
			obj = obj.add(line.split(columnSeparator));

		});

		file.close;
		^obj
	}

	writeFile{ arg path, columnSeparator=$ , rowSeparator=$\n;
		var file, line, lastRow;

		file = File.new(path, "w");
		if( file.isOpen == false, { ^nil	});

		lastRow = this.size - 1;

		this.size.do({ arg row;
			line = this[row][0].asString;

			if(this[row].size > 1, {
				for(1, this[row].size-1, { arg col;
					line = line ++ columnSeparator ++ this[row][col].asString;
				});
			});

			// the last row doesn't need a separator
			if(row != lastRow, {
				line = line ++ rowSeparator;
			});

			file.write(line);
		});

		file.close;
	}
}