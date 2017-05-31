How to use partcopy

[usage] : 
   partcopy infile offset length outfile [offset2] [-yes]
   partcopy infile -allfrom startoffset outfile ...
   partcopy infile -fromto startoffset endoffset outfile ...

[options] : 
   -quiet    do not print number of copied bytes.
   -verbose  tell how offsets and lengths are calculated.
             type directly after "partcopy" for all infos.
   -fromto   use this if you don't want to specify offset
             and length for the input, but a start and end
             offset (with length being end minus start).
   -allfrom  copy all from start offset, until end of file.
   -noext    do not allow an extension of the output file,
             stop with error if writing beyond output end.
   -append   if given after output filename, data is added
             to the end of the output file.

[description] : 
   reads a block of bytes from infile at specified offset,
   writing them into another file. if offset2 is specified,
   outfile is not created, but the bytes are used to overwrite
   other bytes within the output file.

   by default, the command simulates, checking file lengths
   but not copying any data. add -yes to really copy contents.

   negative offsets can be used, to specify positions
   relative to the file end instead of file start.

   if output offset2 + length is larger than output file size,
   the output file is extended. use -noext to avoid this.

   length syntax:
      100000   ,  100k        = 100000 bytes
      2000000k , 2000m , 2g   = 2 giga-bytes

[examples] : 
   partcopy first.dat 10000 50 second.dat 500000
     - copies 50 bytes from first.dat at offset 10000 into
       second.dat (which should exist) at offset 500000

   partcopy first.dat -fromto 0x2710 0x2950 second.dat
     - input start and end offset in hexadecimal numbers,
       output file is created (no output offset given).

   partcopy in.txt -allfrom 1000 out.txt -append
     - take from in.txt all data from offset 1000 until end
       and append that to out.txt

   partcopy in.dat -fromto -1000 -900 out.dat -500
     - take from in.dat all bytes from end minus 1000
       until end minus 900, i.e. a 100 bytes block, and
       write this into output at end minus 500 bytes.
       (type -verbose after partcopy for details.)
       
