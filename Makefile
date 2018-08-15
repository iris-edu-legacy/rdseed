# Makefile for program rdseed (read FDSN SEED format tapes)
# Main directory
# Revised 19 August 1989 by Dennis O'Neill
#         30 Jul 1991 by Allen Nance
#	  03/25/1999 by Stephane Zuzlewski

# This makefile rebuilds the executable program "rdseed".  All component
# procedures are in various subdirectories and are compiled into a library
# by this and their own makefiles.  The general user/programmer should
# probably be concerned only with subdirectory "Main", which contains the
# main program "rdseed" and the output module "output_data"; other
# subdirectories contain modules which (famous last words) should not need
# modification very often.

# Macro definitions
# EXECUTABLE = name of executable program
# INCLUDE    = location of project include files; used in subdirectories
# LIBRARY    = name of project object library
# CFLAGS     = compilation options; use -O line for production version, -g line
#              for profiling with "gprof (1)"
# MAKE       = command line "make" options passed to Makefiles in subdirectories
# LIBS       = system libraries to link in when constructing executable

CC = cc 

# for cygwin add the -D_CYGWIN flag, for users of windows pcs
#CFLAGS     = -O -m32 -g -D_CYGWIN

# to compile rdseed as a 32-bit application
#CFLAGS     = -O -m32 -g 

# uncomment to accommodate earlier versions of Mac OS X
#CFLAGS = -O -m64 -g -mmacosx-version-min=10.4

#else
CFLAGS = -O -m64 -g

INCLUDE    = -I../Include

CFLAGSLINE = "$(INCLUDE) $(CFLAGS)"

# comment out one or the other of the LDFLAGS

# Default LDFLAGS should be good for Linux, Mac OSX, PC etc.
LDFLAGS = -lm -lc

# Uncomment this line for Solaris
#LDFLAGS = -lm -lc -lnsl

MAKE = make CFLAGS=${CFLAGSLINE} all

OBJS = 	Main/ah_resp.o Main/output_data.o Main/output_sac.o Main/alt_response.o Main/output_LOG.o Main/output_ah.o     Main/output_mini.o    Main/rdseed.o\
		Main/output_simple.o Main/output_css.o    Main/output_resp.o    Main/time_span_out.o Main/steim.o Main/output_seed.o Main/output_event_info.o \
		Main/summary.o Main/output_segy.o Main/output_stn_chn_comments.o Main/output_b2k.o Main/atc.o \
		Ah/ioroutin.o\
	Decoders/convert_seedhdr.o Decoders/decode_echery.o Decoders/decode_graef.o Decoders/decode_rstn.o Decoders/decode_32bit.o Decoders/decode_16bit.o \
        	Decoders/decode_ieeefloat.o Decoders/decode_ieeedouble.o Decoders/decode_asro.o Decoders/decode_cdsn.o Decoders/decode_dwwssn.o Decoders/decode_sro.o Decoders/decode_steim.o \
        	Decoders/decode_geoscope.o Decoders/decode_ddl.o Decoders/decode_steim2.o Decoders/init_data_hdr.o Decoders/process_time_span.o \
		Decoders/process_event_requests.o Decoders/process_data.o Decoders/decode_usnsn.o \
	Parsers/get_response.o Parsers/parse_key.o Parsers/parse_10.o Parsers/parse_11.o Parsers/parse_12.o Parsers/parse_30.o Parsers/parse_31.o Parsers/parse_32.o \
		Parsers/parse_33.o Parsers/parse_34.o Parsers/parse_35.o Parsers/parse_41.o Parsers/parse_42.o Parsers/parse_43.o Parsers/parse_44.o \
		Parsers/parse_45.o Parsers/parse_46.o Parsers/parse_47.o Parsers/parse_48.o Parsers/parse_50.o Parsers/parse_51.o Parsers/parse_52.o \
		Parsers/parse_53.o Parsers/parse_54.o Parsers/parse_55.o Parsers/parse_56.o Parsers/parse_57.o Parsers/parse_58.o Parsers/parse_59.o \
		Parsers/parse_60.o Parsers/parse_61.o Parsers/parse_62.o Parsers/parse_70.o Parsers/parse_71.o Parsers/parse_72.o Parsers/parse_73.o \
		Parsers/parse_74.o Parsers/parse_100.o Parsers/parse_2k.o Parsers/free_all.o Parsers/free_response.o Parsers/free_11.o Parsers/free_12.o \
		Parsers/free_30.o Parsers/free_31.o Parsers/free_32.o Parsers/free_33.o Parsers/free_34.o Parsers/free_35.o Parsers/free_41.o Parsers/free_42.o \
		Parsers/free_43.o Parsers/free_44.o Parsers/free_45.o Parsers/free_46.o Parsers/free_47.o Parsers/free_48.o Parsers/free_50.o \
		Parsers/free_51.o Parsers/free_52.o Parsers/free_53.o Parsers/free_54.o Parsers/free_55.o Parsers/free_56.o Parsers/free_57.o Parsers/free_58.o \
		Parsers/free_59.o Parsers/free_60.o Parsers/free_61.o Parsers/free_62.o Parsers/free_70.o Parsers/free_71.o Parsers/free_72.o Parsers/free_73.o \
		Parsers/free_74.o Parsers/free_2k.o Parsers/prc_abrvd.o Parsers/prc_blank.o Parsers/prc_stnh.o Parsers/prc_timeh.o Parsers/prc_volh.o \
	Printers/pagetop.o Printers/print_abrvd.o Printers/print_channel.o Printers/print_response.o Printers/print_stnh.o Printers/print_timeh.o \
		Printers/print_key.o Printers/print_10.o Printers/print_11.o Printers/print_12.o Printers/print_30.o Printers/print_31.o Printers/print_32.o \
		Printers/print_33.o Printers/print_34.o Printers/print_35.o Printers/print_41.o Printers/print_42.o Printers/print_43.o Printers/print_44.o \
		Printers/print_45.o Printers/print_46.o Printers/print_47.o Printers/print_48.o Printers/print_50.o Printers/print_51.o Printers/print_52.o \
		Printers/print_53.o Printers/print_54.o Printers/print_55.o Printers/print_56.o Printers/print_57.o Printers/print_58.o Printers/print_59.o \
		Printers/print_60.o Printers/print_61.o Printers/print_62.o Printers/print_70.o Printers/print_71.o Printers/print_72.o Printers/print_73.o \
		Printers/print_74.o Printers/find_30.o Printers/find_31.o Printers/find_32.o Printers/find_33.o Printers/find_34.o Printers/find_41.o \
	 	Printers/find_42.o Printers/find_43.o Printers/find_44.o Printers/find_45.o Printers/find_46.o Printers/find_47.o Printers/find_48.o \
		Printers/print_volh.o \
	Utilities/alloc_linklist_element.o Utilities/allocation_error.o Utilities/cmdlineproc.o Utilities/find_wordorder.o Utilities/memncpy.o \
		Utilities/parse_double.o Utilities/parse_int.o Utilities/parse_long.o Utilities/parse_nchar.o Utilities/parse_varlstr.o \
		Utilities/read_blockette.o Utilities/read_logical_record.o Utilities/split.o Utilities/swap_2byte.o Utilities/swap_4byte.o Utilities/timeadd.o \
		Utilities/timeadd_double.o Utilities/timeaddphase.o Utilities/timedif.o Utilities/timecvt.o Utilities/timeprt.o Utilities/timecmp.o \
		Utilities/timetol.o Utilities/timeqc.o Utilities/timepch.o Utilities/log_errors.o Utilities/strlst.o Utilities/get_date.o Utilities/delaz.o \
	Snoop/times_data.o

rdseed : Main_dir Ah_dir Decoders_dir Parsers_dir Printers_dir Utilities_dir Snoop_dir
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LDFLAGS} 

Main_dir :
	cd Main; $(MAKE)

Ah_dir :
	cd Ah; $(MAKE)

Decoders_dir :
	cd Decoders; $(MAKE)

Parsers_dir :
	cd Parsers; $(MAKE)

Printers_dir :
	cd Printers; $(MAKE)

Utilities_dir :
	cd Utilities; $(MAKE)

Snoop_dir :
	cd Snoop; $(MAKE)

clean:
		-cd Include;   echo "Cleaning Include . . . . "; rm *.o; rm *%; cd ..
		-cd Main;      echo "Cleaning Main  . . . . . "; rm *.o; rm *%; cd ..
		-cd Decoders;  echo "Cleaning Decoders  . . . "; rm *.o; rm *%; cd ..
		-cd Parsers;   echo "Cleaning Parsers . . . . "; rm *.o; rm *%; cd ..
		-cd Printers;  echo "Cleaning Printers  . . . "; rm *.o; rm *%; cd ..
		-cd Snoop;     echo "Cleaning Snoop . . . . . "; rm *.o; rm *%; cd ..
		-cd Utilities; echo "Cleaning Utilities . . . "; rm *.o; rm *%; cd ..
		-cd Ah;        echo "Cleaning Ah  . . . . . . "; rm *.o; rm *%; cd ..
		echo "Finished."
