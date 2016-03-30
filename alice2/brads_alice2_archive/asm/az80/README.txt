/*
	HEADER:		CUG276;
	TITLE:		Can't Stop Cross-Assembling Portably;
	FILENAME:	READ.ME5;
	VERSION:	0.0;
	DATE:		07/12/1989;

	DESCRIPTION:	"This is the general introduction file for the 6804,
			PIC1650, and Z-80 cross-assemblers.  Each assembler is
			explained in detail in its header file.";

	SEE-ALSO:	6804 Cross-Assembler (Portable),
			PIC1650 Cross-Assembler (Portable),
			Z-80 Cross-Assembler (Portable);

	AUTHORS:	William C. Colley III;
*/

     What's on this disk?  It goes like this:

	READ.ME5	This notice.

	A684.DOC	The manual for the 6804 cross-assembler.  This file
			can be printed with the following command line under
			CP/M-80:

				A>pip lst:=a684.doc[t8]

	A684.H		The source files for the 6804 cross-assembler.  The
	A684.C		file A684.H also contains compile, assemble, and link
	A684EVAL.C	instructions for all compilers that I have used with
	A684UTIL.C	the package or that users have told me how to use with
			the package.

	TEST684.ASM	The entire instruction set of the 6804 processor to
			use in testing the 6804 cross-assembler.

	A16.DOC		The manual for the PIC1650 cross-assembler.  This file
			can be printed with the following command line under
			CP/M-80:

				A>pip lst:=a16.doc[t8]

	A16.H		The source files for the PIC1650 cross-assembler.  The
	A16.C		file A16.H also contains compile, assemble, and link
	A16EVAL.C	instructions for all compilers that I have used with
	A16UTIL.C	the package or that users have told me how to use with
			the package.

	TEST16A.ASM	The selections from the instruction set of the
	TEST16B.ASM	PIC16C52 CPU to use in testing the PIC1650 cross-
	TEST16C.ASM	assembler.

	AZ80.DOC	The manual for the Z-80 cross-assembler.  This file
			can be printed with the following command line under
			CP/M-80:

				A>pip lst:=az80.doc[t8]

	AZ80.H		The source files for the Z-80 cross-assembler.  The
	AZ80.C		file AZ80.H also contains compile, assemble, and link
	AZ80EVAL.C	instructions for all compilers that I have used with
	AZ80UTIL.C	the package or that users have told me how to use with
			the package.

	TESTZ80.ASM	The entire instruction set of the Z-80 CPU to use in
			testing the Z-80 cross-assembler.

	Some future cross-assembler will fill out the disk.

     I not only wrote these pieces of software originally, but I am also the
librarian for the disk, and as such, I am responsible for maintaining the
packages.  I can only do this with YOUR help.  PLEASE report bugs to me
promptly so that I can get them fixed.  I am only a letter or a phone call
away.

     The lists of compile, assemble, and link instructions for the packages
are limited by my personal experience if YOU do not contribute.  In
particular, I have no access to an AT&T UNIX system (only clones of BSD 4.2
UNIX) nor to IBM-PC C compilers other than Eco-C88, Microsoft C 3.0, Turbo C,
and DeSmet C. I need YOU to tell me how to use the package with these systems
and others.  One note:  The setting for the Microsoft compiler is the vanilla
setting.  It works for Turbo C and most other modern compilers.

     Once the first cross-assembler (the 6801 package) was written, adapting
it to other 8-bit or 4-bit processors (like the 1805A) was easy.  In fact, I
once adapted one of the BDS C versions to a 12-bit processor (the PIC1650). It
only takes a couple of evenings for me to do it. What processors should be
next, if any?  Get your votes in, either to me or to the Users' Group.
Otherwise, the selection of cross-assemblers will reflect only my needs, which
at the moment are taking off in a bizarre directions -- toward very small
machines like the 6804 and toward obsolete machines like the 2650.

     Having the author double as the librarian allows this disk to be a
living, growing entity, but its growth will be stunted by my lack of
resources if YOU do not contribute your share.  Speak up, for everybody's
sake!

						William C. Colley, III
						14 Hawthorne Drive
						Oberlin, OH 44074

						ph (216)288-1122 x469 (day)
						   (216)775-3680      (nite)
