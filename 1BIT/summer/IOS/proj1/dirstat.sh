#!/bin/sh

########################################################################
# IOS - Operacni systemy
# Projekt 1
# xtotha01

_Dir_Check()
{
    if [ ! -d $1 ]; then
        echo "Directory does not exits! \"$0\" aborted!"
        exit 1
    fi
}

_Get_Dir()
{
    if [ `echo $1 | cut -c 1` = "/" ]; then
        r_dir=$1
    else
        r_dir=`pwd`/$1
    fi
}

_Print_Error()
{
    echo "$1"
    exit 1
}

_Check_File_Ere()
{
    if [ `uname` = "Linux" ]; then
        _posix="-W posix"
    else
        _posix=""
    fi
    echo $1 | awk $_posix -v _ERE=$file_ere 'BEGIN{FS="/";_EXISTS=0}{ y=1;
                                                                       while ( y < (NF+1) ) {
                                                                                            if  ( $y ~ _ERE ) { _EXISTS=1 ; exit}
                                                                                            y++
                                                                                            }
                                                                     }
                                              END{  if ( _EXISTS == 0 ) { print $0 }}'
}


_Get_Max_Depth()

{

if [ $fullpath -eq 1 ]; then

       _dir="$r_dir"

    else

       _dir="$(pwd)"

    fi

if [ `pwd` = $_dir ] ; then

  default_full_path=1

else

default_full_path=0

fi

    awk -v awk_pwd=$_dir -v awk_deffullpath=$_default_full_path 'BEGIN{

                            FS="/";

                            MAXDEPTH=0;

                                { sub("/$","",awk_pwd) }

                            awk_pwd_count = (gsub("/","#",awk_pwd)-1)

                            }

                            {

                            _Rec=$0

                            sub("/$","",_Rec)

                            if ( awk_deffullpath == 1 ) { gsub(awk_pwd,"",_Rec) }

                            _finded = gsub("/","#",_Rec)

                            if (  _finded > MAXDEPTH ) { MAXDEPTH=_finded }

                            }

                            END { print (MAXDEPTH - awk_pwd_count)

                            }'

}

_DirInfoReport()
{
# ND = Directories #####################################################
if [ $file_ere ]; then
    directories_count=$(find "$r_dir" -type d | while read _path ; do _Check_File_Ere $_path ; done | wc -l)
else
    directories_count=$(find "$r_dir" -type d | wc -l)
fi

# DD = Max depth #######################################################
if [ $file_ere ]; then
    max_depth=$(find "$r_dir" -type d | while read _path ; do _Check_File_Ere $_path ; done | _Get_Max_Depth)
    if [ $max_depth -lt 0 ]; then
    	max_depth=0
    fi
else
    max_depth=$(find "$r_dir" -type d | _Get_Max_Depth)
fi

# NF = All files #######################################################
if [ $file_ere ]; then
    all_files=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | wc -l)
else
    all_files=$(find "$r_dir" -type f | wc -l)
fi

# AF = Average no. of files ############################################
if [ $directories_count -eq 0 ]; then
    average_no_of_files=0
else
    average_no_of_files=$(expr $all_files / $directories_count)
fi

# LF = Largest file ####################################################
if [ $file_ere ]; then
    largest_file=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | while read _path
                                                                                                 do qww=$(find $_path -type f -ls | awk '{print $7}')
                                                                                                 printf "%s\n" "$qww"
                                                                                                 done | sort -rn | head -1)
else
    largest_file=$(find "$r_dir" -type f -ls | awk '{print $7}' | sort -rn | head -1)
fi
if [ ! $largest_file ]; then
   largest_file="N/A"
elif [ $largest_file -eq 0 ]; then
	largest_file="N/A"
fi

# AS = Average file size ###############################################
if [ $file_ere ]; then
    size_of_files=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | while read _path
                                                                                                  do qww=$(find $_path -type f -ls | awk '{print $7}')
                                                                                                  printf "%s\n" "$qww"
                                                                                                  done | awk '{sum += $1} END {print sum}')
    if [ $all_files -eq 0 ]; then
        average_file_size="N/A"
    else
    average_file_size=$(expr $size_of_files / $all_files)
    fi
else
    size_of_files=$(find "$r_dir" -type f -ls | awk '{sum += $7} END {print sum}')
    if [ $all_files -eq 0 ]; then
        average_file_size="N/A"
    else
    average_file_size=$(expr $size_of_files / $all_files)
    fi
fi

# MS = File size median ################################################
if [ $file_ere ]; then
    count=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | wc -l)
    c_num=$(expr $count % 2)
    if [ $all_files -eq 0 ]; then
        file_size_median="N/A"
    else
        if [ $c_num -eq 0 ]; then
            count_one=$(expr $count / 2 )
            count_two=$(expr $count_one + 1)
            file_size_median_one=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | while read _path
                                                                                                                 do qww=$(find $_path -type f -ls | awk '{print $7}')
                                                                                                                 printf "%s\n" "$qww"
                                                                                                                 done | sort -rn | head -n $count_one | tail -1)
            file_size_median_two=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | while read _path
                                                                                                                 do qww=$(find $_path -type f -ls | awk '{print $7}')
                                                                                                                 printf "%s\n" "$qww"
                                                                                                                 done | sort -rn | head -n $count_two | tail -1)
            middle=$(expr $file_size_median_one + $file_size_median_two)
            file_size_median=$(expr $middle / 2)
        else
            count=$(expr $count + 1)
            count=$(expr $count / 2 )
            file_size_median=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | while read _path
                                                                                                                 do qww=$(find $_path -type f -ls | awk '{print $7}')
                                                                                                                 printf "%s\n" "$qww"
                                                                                                                 done | sort -rn | head -n $count | tail -1)
        fi
    fi
else
    count=$(find "$r_dir" -type f | wc -l)
    c_num=$(expr $count % 2)
    if [ $all_files -eq 0 ]; then
        file_size_median="N/A"
    else
        if [ $c_num -eq 0 ]; then
            count_one=$(expr $count / 2 )
            count_two=$(expr $count_one + 1)
            file_size_median_one=$(find "$r_dir" -type f -ls | awk '{print $7}' | sort -rn | head -n $count_one | tail -1)
            file_size_median_two=$(find "$r_dir" -type f -ls | awk '{print $7}' | sort -rn | head -n $count_two | tail -1)
            middle=$(expr $file_size_median_one + $file_size_median_two)
            file_size_median=$(expr $middle / 2)
        else
            count=$(expr $count + 1)
            count=$(expr $count / 2 )
            file_size_median=$(find "$r_dir" -type f -ls | awk '{print $7}' | sort -rn | head -n $count | tail -1)
        fi
    fi
fi

# EL = File extensions ######################################################
if [ $file_ere ]; then
    file_extensions=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | sed "s/^.*[^\/]\.//" | sed "s/.*\/\..*//" | grep '^[[:alnum:]]*$' | sort | uniq)
    file_extensions_print_in_line=$(echo $file_extensions | sed s/' '/','/g | sed "s/,$//")
else
    file_extensions=$(find "$r_dir" -type f | sed "s/^.*[^\/]\.//" | sed "s/.*\/\..*//" | grep '^[[:alnum:]]*$' | sort | uniq)
    file_extensions_print_in_line=$(echo $file_extensions | sed s/' '/','/g | sed "s/,$//")
fi

# Report PART =1= ######################################################
printf "Root directory: %s\n" "$input_dir"
printf "Directories: %s\n" $directories_count
printf "Max depth: %s\n" $max_depth
printf "Average no. of files: %s\n" $average_no_of_files
printf "All files: %s\n" $all_files
printf "  Largest file: %s\n" $largest_file
printf "  Average file size: %s\n" $average_file_size
printf "  File size median: %s\n" $file_size_median
printf "File extensions: %s\n" $file_extensions_print_in_line

# EXT = File extensions ######################################################
if [ $file_ere ]; then
    file_extensions_count=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | sed "s/^.*[^\/]\.//" | sed "s/.*\/\..*//" | grep '^[[:alnum:]]*$' | sort | uniq)
    if [ ! file_extensions_count ]; then
        exit 0
    fi
    for i in `echo $file_extensions_count`
    do
        FE_count=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | sed "s/^.*[^\/]\.//" | sed "s/.*\/\..*//" | grep '^[[:alnum:]]*$' | grep "^$i$" | wc -l)
        printf "Files .%s: %s\n" "$i" $FE_count
        FE_largest_file=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | while read _path
                                                                                                        do qww=$(find $_path -type f -ls | grep "\.$i$" | awk '{print $7}')
                                                                                                        printf "%s\n" "$qww"
                                                                                                        done | sort -rn | head -1)
        printf "  Largest file .%s: %s\n" "$i" $FE_largest_file
        FE_size_of_files=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | while read _path
                                                                                                         do qww=$(find $_path -type f -ls | grep "\.$i$" | awk '{print $7}')
                                                                                                         printf "%s\n" "$qww"
                                                                                                         done | awk '{sum += $1} END {print sum}')
        if [ $FE_count -eq 0 ]; then
            FE_average_file_size=0
        else
            FE_average_file_size=$(expr $FE_size_of_files / $FE_count)
        fi
        printf "  Average file size .%s: %s\n" "$i" $FE_average_file_size
        FE_c_num=$(expr $FE_count % 2)
        if [ $FE_c_num -eq 0 ]; then
            FE_count_one=$(expr $FE_count / 2 )
            FE_count_two=$(expr $FE_count_one + 1)
            FE_file_size_median_one=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | while read _path
                                                                                                             do qww=$(find $_path -type f -ls | grep "\.$i$" | awk '{print $7}')
                                                                                                             printf "%s\n" "$qww"
                                                                                                             done | sort -rn | head -n $FE_count_one | tail -1)
            FE_file_size_median_two=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | while read _path
                                                                                                             do qww=$(find $_path -type f -ls | grep "\.$i$" | awk '{print $7}')
                                                                                                             printf "%s\n" "$qww"
                                                                                                             done | sort -rn | head -n $FE_count_two | tail -1)
            FE_middle=$(expr $FE_file_size_median_one + $FE_file_size_median_two)
            FE_file_size_median=$(expr $FE_middle / 2)
        else
            FE_count=$(expr $FE_count + 1)
            FE_count=$(expr $FE_count / 2 )
            FE_file_size_median=$(find "$r_dir" -type f | while read _path ; do _Check_File_Ere $_path ; done | while read _path
                                                                                                                do qww=$(find $_path -type f -ls | grep "\.$i$" | awk '{print $7}')
                                                                                                                printf "%s\n" "$qww"
                                                                                                                done | sort -rn | head -n $FE_count | tail -1)
        fi
        printf "  File size median .%s: %s\n" "$i" $FE_file_size_median
    done
else
    file_extensions_count=$(find "$r_dir" -type f | sed "s/^.*[^\/]\.//" | sed "s/.*\/\..*//" | grep '^[[:alnum:]]*$' | sort | uniq)
    if [ ! file_extensions_count ]; then
        exit 0
    fi
    for i in `echo $file_extensions_count`
    do
    	FE_count=$(find "$r_dir" -type f | sed "s/^.*[^\/]\.//" | sed "s/.*\/\..*//" | grep '^[[:alnum:]]*$' | grep "^$i$" | wc -l)
        printf "Files .%s: %s\n" "$i" $FE_count
        FE_largest_file=$(find "$r_dir" -type f -ls | awk {'printf ("%s\t%s\n", $7, $11)'} | grep "\.$i$" | awk '{print $1}' | sort -rn | head -1 )
        printf "  Largest file .%s: %s\n" "$i" $FE_largest_file
        FE_size_of_files=$(find "$r_dir" -type f -ls | awk {'printf ("%s\t%s\n", $7, $11)'} | grep "\.$i$" | awk '{print $1}' | awk '{sum += $1} END {print sum}')
        if [ $FE_count -eq 0 ]; then
            FE_average_file_size=0
        else
            FE_average_file_size=$(expr $FE_size_of_files / $FE_count)
        fi
        printf "  Average file size .%s: %s\n" "$i" $FE_average_file_size
        FE_c_num=$(expr $FE_count % 2)
        if [ $FE_c_num -eq 0 ]; then
            FE_count_one=$(expr $FE_count / 2 )
            FE_count_two=$(expr $FE_count_one + 1)
            FE_file_size_median_one=$(find "$r_dir" -type f -ls | awk {'printf ("%s\t%s\n", $7, $11)'} | grep "\.$i$" | awk '{print $1}' | sort -rn | head -n $FE_count_one | tail -1)
            FE_file_size_median_two=$(find "$r_dir" -type f -ls | awk {'printf ("%s\t%s\n", $7, $11)'} | grep "\.$i$" | awk '{print $1}' | sort -rn | head -n $FE_count_two | tail -1)
            FE_middle=$(expr $FE_file_size_median_one + $FE_file_size_median_two)
            FE_file_size_median=$(expr $FE_middle / 2)
        else
            FE_count=$(expr $FE_count + 1)
            FE_count=$(expr $FE_count / 2)
            FE_file_size_median=$(find "$r_dir" -type f -ls | awk {'printf ("%s\t%s\n", $7, $11)'} | grep "\.$i$" | awk '{print $1}' | sort -rn | head -n $FE_count | tail -1)
        fi
		printf "  File size median .%s: %s\n" "$i" $FE_file_size_median
    done
fi

} # _DirInfoReport END

# MAIN PROGRAM #########################################################
case $# in
    # dirstat
    0)  r_dir="$(pwd)"
        input_dir="$(pwd)"
        fullpath=0
        ;;
    # dirstat [DIR]
    1)  _Get_Dir $1
        _Dir_Check $r_dir
        input_dir="$1"
        fullpath=1
        ;;
    # dirstat [-i FILE_ERE]
    2)  if [ "$1" = "-i" ]; then
            if [ "$2" = "/" ]; then
                _Print_Error "FILE_ERE cannot be a root directory! \"$0\" aborted!"
            else
            r_dir="$(pwd)"
            file_ere="$2"
            input_dir="$(pwd)"
            fullpath=0
            fi
        else
            _Print_Error "Unexpected arguments. \"$0\" aborted!"
        fi
        ;;
    # dirstat [-i FILE_ERE] [DIR]
    3)  if [ "$1" = "-i" ]; then
            if [ "$2" = "/" ]; then
                _Print_Error "FILE_ERE cannot be a root directory! \"$0\" aborted!"
            else
            _Get_Dir $3
            _Dir_Check $r_dir
            file_ere="$2"
            input_dir="$3"
            fullpath=1
            fi
        else
            _Print_Error "Unexpected arguments. \"$0\" aborted!"
        fi
        ;;
    *)  _Print_Error "Unexpected arguments. \"$0\" aborted!"
        ;;
esac

_DirInfoReport
exit 0