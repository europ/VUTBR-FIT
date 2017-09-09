<?php

###########################################################

# IPP - Principles of Programming Languages
# Project 1
# Adrian Toth
# xtotha01

###########################################################

// for safety of my output
error_reporting(0);

// global variables (flags)
$flag_i=false;  // input file_or_dir
$flag_o=false;  // output file
$flag_p=false;  // pretty-xml K
$flag_ni=false; // no-inline
$flag_m=false;  // max-par N
$flag_nd=false; // no-duplicates
$flag_r=false;  // remove-whitespace

// global variables which are given behind the options
$input_FoD="";
$output_F="";
$px_K;
$mp_N;

// other global variables
$output;
$xmlWriter = new XMLWriter();


// main fucntion call in which are called other functions
main($argv,$argc);

###########################################################

// function prints help message
function print_help() {
	fprintf(STDOUT,
"
Script's parameters:

	Input destination:
	[-i name][--input=name] where name represents file or directory

	Output destination:
	[-o name][--output=name] where name represents file

	Offset size:
	[-p K][--pretty-xml=K] where K is a decimal number, if K is not entered, than it is set on value 4

	Ignore inline functions:
	[-ni][--no-inline]

	Consider only functions which have less or equal parameters:
	[-m N][--max-par=N] where N is a decimal number

	Ignore duplicated functions:
	[-nd][--no-duplicates]

	Remove whitespace from rettype and type:
	[-r][--remove-whitespace]

	Help:
	[-h][--help]

");
}

// function terminate the script with exitcode after printing an error message
function error($string, $exitcode) {
	fprintf(STDERR, "$string\n");
	exit($exitcode);
}

// this function is used when i have to support short parameters (assigment has change => not supporting)
// function returns true if the string represents a parameter, false if not
function is_opt($string) {
	$regex_arr=["/^--input=.*$/u", "/^--output=.*$/u", "/^--pretty-xml=.*$/u", "/^--no-inline$/u", "/^--max-par=.*$/u", "/^--no-duplicates$/u", "/^--remove-whitespace$/u", "/^-i$/u", "/^-o$/u", "/^-p$/u", "/^-ni$/u", "/^-m$/u", "/^-nd$/u", "/^-r$/u"];
	foreach ($regex_arr as $regex) {
		if (preg_match($regex, $string) == 1) {
			return true;
		}
	}
	return false;
}

// function provide examination of param
function check_parameters(array $argv, $argc) {

	global $flag_i, $flag_o, $flag_p, $flag_ni, $flag_m, $flag_nd, $flag_r;
	global $input_FoD, $output_F, $px_K, $mp_N;

	// script is launched with --help
	if ($argc == 2) {
		if ($argv[1]=="--help") {
			print_help();
			exit(0);
		}
	}

	// script is launched with -h
	if ($argc == 2) {
		if ($argv[1]=="-h") {
			print_help();
			exit(0);
		}
	}

	unset($argv[0]); // erase useless script name from args

	// iterate trought parameters and check them
	for ($i = 1; $i < $argc; $i++) {

		/* if parameter match the regex, set flag (on true)
		   if param does not mach any regex => unknown param
		   if it match short param then check the following, if the following is not param than load it
		*/
		switch ($argv[$i]) {

			// LONG OPTIONS =========================================================================

			case (preg_match("/^--input=.*$/u", $argv[$i]) ? true : false) :
				if ($flag_i==false) {
					$flag_i=true;
					$input_FoD=preg_replace("/^--input=/u", '',$argv[$i]);
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			case (preg_match("/^--output=.*$/u", $argv[$i]) ? true : false) :
				if ($flag_o==false) {
					$flag_o=true;
					$output_F=preg_replace("/^--output=/u", '',$argv[$i]);
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			// phorum forced me to do this
			case (preg_match("/^--pretty-xml.*$/u", $argv[$i]) ? true : false) :
				if ($flag_p==false) {
					$flag_p=true;
					if (preg_match("/^--pretty-xml$/u", $argv[$i])==1) {
						;
					}
					else {
						if (preg_match("/^--pretty-xml=$/u", $argv[$i])==1) {
							error("Wrong parameters! ([--pretty-xml=k] where k is a decimal number)", 1);
						}
						else {
							$px_K=preg_replace("/^--pretty-xml=/u", '',$argv[$i]);
							if (preg_match("/^[[:digit:]]*$/u", $px_K) != 1 ) {
								error("Wrong parameters! ([--pretty-xml=k] where k is a decimal number)", 1);
							}
						}
					}
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			case (preg_match("/^--no-inline$/u", $argv[$i]) ? true : false) :
				if ($flag_ni==false) {
					$flag_ni=true;
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			case (preg_match("/^--max-par=.*$/u", $argv[$i]) ? true : false) :
				if ($flag_m==false) {
					$flag_m=true;
					$mp_N=preg_replace("/^--max-par=/u", '',$argv[$i]);
					if (preg_match("/^[[:digit:]]*$/u", $mp_N) != 1 ) {
						error("Wrong parameters! ([--max-par=n] where n is a decimal number)", 1);
					}
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			case (preg_match("/^--no-duplicates$/u", $argv[$i]) ? true : false) :
				if ($flag_nd==false) {
					$flag_nd=true;
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			case (preg_match("/^--remove-whitespace$/u", $argv[$i]) ? true : false) :
				if ($flag_r==false) {
					$flag_r=true;
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			// SHORT OPTIONS ========================================================================

			case (preg_match("/^-i$/u", $argv[$i]) ? true : false) :
				if ($flag_i==false) {
					$flag_i=true;
					if (($i+1) < $argc) {
						if (is_opt($argv[$i+1])==false) {
							$input_FoD=$argv[$i+1];
							$i++;
						}
					}
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			case (preg_match("/^-o$/u", $argv[$i]) ? true : false) :
				if ($flag_o==false) {
					$flag_o=true;
					if (($i+1) < $argc) {
						if (is_opt($argv[$i+1])==false) {
							$output_F=$argv[$i+1];
							$i++;
						}
					}
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			case (preg_match("/^-p$/u", $argv[$i]) ? true : false) :
				if ($flag_p==false) {
					$flag_p=true;
					if (($i+1) < $argc) {
						if (is_opt($argv[$i+1])==false) {
							if (preg_match("/^[[:digit:]]*$/u", $argv[$i+1])) {
								$px_K=$argv[$i+1];
								$i++;
							}
							else {
								error("Wrong parameters! ([-p k] where k is a decimal number)", 1);
							}
						}
					}
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			case (preg_match("/^-ni$/u", $argv[$i]) ? true : false) :
				if ($flag_ni==false) {
					$flag_ni=true;
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			case (preg_match("/^-m$/u", $argv[$i]) ? true : false) :
				if ($flag_m==false) {
					$flag_m=true;
					if (($i+1) < $argc) {
						if (is_opt($argv[$i+1])==false) {
							if (preg_match("/^[[:digit:]]*$/u", $argv[$i+1])) {
								$mp_N=$argv[$i+1];
								$i++;
							}
							else {
								error("Wrong parameters! ([-m n] where n is a decimal number)", 1);
							}
						}
					}
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			case (preg_match("/^-nd$/u", $argv[$i]) ? true : false) :
				if ($flag_nd==false) {
					$flag_nd=true;
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			case (preg_match("/^-r$/u", $argv[$i]) ? true : false) :
				if ($flag_r==false) {
					$flag_r=true;
				}
				else {
					error("Duplicated parameters!", 1);
				}
				break;

			default :
				error("Unknown parameters!", 1);

		}
	}

	// condition from assignment, you have to enter this param
	if ($flag_m) {
		if ( (!$mp_N) && !($mp_N==="0") ) {
			error("[-m n] [--max-par=n] N must be entered!",1);
		}
	}

	// condition from assignment, if K is not entered than it is set on default value 4
	if ($flag_p) {
		if ($px_K==="0") {
			;
		}
		else {
			if (!$px_K) {
				$px_K=4;
			}
		}
	}
}

// erase { \r }
function remove_carriage_return($str) {
	return preg_replace("/\\r/u", "", $str);
}

// erase { \" }
function remove_backslash_with_quote($str) {
	return preg_replace("/[\\\]\"/u", '', $str);
}

// erase { \' }
function remove_backslash_with_apostrophe($str) {
	return preg_replace("/[\\\]\'/u", '', $str);
}

// erase { /*...*/ }
function remove_block_comments($str) {
	return preg_replace("/[\/][*].*[*][\/]/umUs", '',$str);
}

// erease { "..." }
function remove_strings($str) {
	return preg_replace("/\".*\"/umUs", '', $str);
}

// erease { '...' }
function remove_strings_in_apostrophes($str) {
	return preg_replace("/\'.*\'/umUs", '', $str);
}

// erase { //...\n }
function remove_line_comments($str) {
	return preg_replace("/\/\/.*\\n/u", '', $str);
}

// erase { //...\\n...\\n...\n }
function remove_multiline_line_comments($str) {
	$i=0;
	$c=substr($str, $i, 1);
	$length=strlen($str);
	$line="";
	$delete="";
	$multiline_macro_detected=false;
	$last_line_of_multiline_macro=false;
	$array_of_strings_to_delete=array();
	while($c || $i<$length) {
		$line=$line.$c;
		if ($c=="\n" || $c==end(str_split($str)) ) { // line loaded
			// COMMENT !!!! WARNING !!!
			if (preg_match("/^[\/][\/].*[\\\]$/u",$line)==1) { // it is first line of multiline macro (#...\), save this line
				$last_line_of_multiline_macro=true;
				$multiline_macro_detected=true;
				$delete=$delete.$line;
			}
			else {
				if (preg_match("/.*[\\\]$/u", $line)==1 && $multiline_macro_detected==true){ // first line was detected of multiline macro, save lines with (...\)
					$delete=$delete.$line;
					$last_line_of_multiline_macro=true;
				}
				else { // line (#...\) and lines with (...\) were saved, we need to save the last line of multiline macro
					if ($last_line_of_multiline_macro) {
						$delete=$delete.$line;
						$multiline_macro_detected=false;
						$last_line_of_multiline_macro=false;
						array_push($array_of_strings_to_delete, $delete); // add this sequence of lines to array of string which will be removed
						$delete="";
					}
				}
			}
			$line="";
		}
		$i++;
		$c=substr($str, $i, 1);
	}
	while(!empty($array_of_strings_to_delete)) { // remove every string representating multiline macro
		$to_kill=array_pop($array_of_strings_to_delete);
		$str=str_replace($to_kill, "", $str);
	}
	return $str;
}

// erase { #define ...\n }
function remove_single_line_macro($str) {
	return preg_replace("/#.*\\n/u", '', $str);
}

// erase { #define ...\\n...\n }
function remove_multiline_macro($str) {
	$i=0;
	$c=substr($str, $i, 1);
	$length=strlen($str);
	$line="";
	$delete="";
	$multiline_macro_detected=false;
	$last_line_of_multiline_macro=false;
	$array_of_strings_to_delete=array();
	while($c || $i<$length) {
		$line=$line.$c;
		if ($c=="\n" || $c==end(str_split($str)) ) { // line loaded
			// WARNING WATCH OUT THIS IS MULTI LINE MACRO !!!!
			// ==== #define MACROOOOOOOOOO ====
			if (preg_match("/^#.*[\\\]$/u",$line)==1) { // it is first line of multiline macro (#...\), save this line
				$last_line_of_multiline_macro=true;
				$multiline_macro_detected=true;
				$delete=$delete.$line;
			}
			else {
				if (preg_match("/.*[\\\]$/u", $line)==1 && $multiline_macro_detected==true){ // first line was detected of multiline macro, save lines with (...\)
					$delete=$delete.$line;
					$last_line_of_multiline_macro=true;
				}
				else { // line (#...\) and lines with (...\) were saved, we need to save the last line of multiline macro
					if ($last_line_of_multiline_macro) {
						$delete=$delete.$line;
						$multiline_macro_detected=false;
						$last_line_of_multiline_macro=false;
						array_push($array_of_strings_to_delete, $delete); // add this sequence of lines to array of string which will be removed
						$delete="";
					}
				}
			}
			$line="";
		}
		$i++;
		$c=substr($str, $i, 1);
	}
	while(!empty($array_of_strings_to_delete)) { // remove every string representating multiline macro
		$to_kill=array_pop($array_of_strings_to_delete);
		$str=str_replace($to_kill, "", $str);
	}
	return $str;
}

// change sequence of newlines { \n...\n } to " "
function replace_newline_with_space($str) {
	return trim(preg_replace("/(\\n)+/u", " ", $str))."\n";
}

// function delete everything what is useless
function erase_rubbish($string) {
	$string=remove_carriage_return($string);           // \r
	$string=remove_backslash_with_quote($string);      // \"
	$string=remove_backslash_with_apostrophe($string); // \'
	$string=remove_block_comments($string);            // /*xxx*/
	$string=remove_strings($string);                   // "xxx"
	$string=remove_strings_in_apostrophes($string);    // 'xxx'
	$string=remove_multiline_line_comments($string);   // //xxx\ \n xxx\ \n xxx
	$string=remove_line_comments($string);             // //xxx \n
	$string=remove_multiline_macro($string);           // #xxx\ \n xxx\ \n xxx
	$string=remove_single_line_macro($string);         // #xxx \n
	$string=replace_newline_with_space($string);       // \n{1,inf} => " "
	return $string;
}

/*
	this function is the heart of the script

	$string = file content as string but for formatted (useless things are removed)
	$file = file path

	from $string it finds functions and makes an array of structures (structure represents one function)
	after this operation it goes trought this array of structures and remove some of them depending on lauching parameters
	the last step is printing this array of structure to final xml
 */
function analyze($string, $file) {

	// variables which will be used below
	global $flag_ni, $flag_nd, $flag_r, $flag_m, $flag_i;
	global $mp_N, $input_FoD;
	global $xmlWriter;
	$id = "[_a-zA-Z][_a-zA-Z0-9]*";

	// find every string which has the function format
	$array=array();
	$regex="/[\s]*(?:[\s]*".$id."[\s\*]+)+".$id."[\s]*[\(][\s\S]*?[\)][\s]*(?:[\{]|[;])/u";
	$count = preg_match_all($regex, $string, $array);

	// reformat the [array of array of strings] to trimmed[array of strings] and replace every \t \n to whitespace
	$array_of_strings=array();
	foreach ($array[0] as $subarray) {
		array_push($array_of_strings, trim(preg_replace("/[\s]/u", " ", $subarray)));
	}

	// remove nonfunction strings from array of string e.g. "esle if (x==true) {"
	foreach ($array_of_strings as $i => $subarray) {
		if (preg_match("/\s*(if|else|return)\s* /u", $subarray)==1) {
			unset($array_of_strings[$i]);
		}
	}


	// IN => array of strings (array of functions)
	//=====================================================================


	// split string representating function into parts
	$diced_function_array=array();
	$structure=array();
	$cutting_regex="/\s*(((?:extern\s+)?)((?:inline\s+)?)(?:\s*".$id."[\s\*]+)+)(".$id.")\s*\(([\s\S]*?)\)\s*(?:[\{]|[;])/us";
	foreach ($array_of_strings as $str) {
		preg_match($cutting_regex, $str, $structure);
		array_push($diced_function_array, $structure);
	}


	//=====================================================================
	// OUT => array of array of strings (array of structures)


	// if --no-duplicates then remove functions with the same name
	if ($flag_nd) {
		$structures_to_remove=array();
		foreach ($diced_function_array as $i => $structure_i) {
			foreach ($diced_function_array as $j => $structure_j) {
				if ($j>$i) { // dont remove yourself
					if ($structure_i[4]===$structure_j[4]) { // we find two duplicates (structure_Fx == structure_Fy)
						array_push($structures_to_remove, $j);
					}
				}
			}
		}
		foreach ($structures_to_remove as $value) {
			unset($diced_function_array[$value]);
		}
	}

	// if --no-inline than remove inline functions
	if ($flag_ni) {
		foreach ($diced_function_array as $key => $structure) {
			if (preg_match("/[\s]*inline[\s]*/u", $structure[3])==1) {
				unset($diced_function_array[$key]);
			}
		}
	}

	// if it is function with multiple paramters than $structure[6] is yes, else no
	foreach ($diced_function_array as $key => $structure) {
		if (preg_match("/[\.]{3}[\s]*$/um", $structure[5])==1) {
			array_push($diced_function_array[$key], "yes");
		}
		else {
			array_push($diced_function_array[$key], "no");
		}
	}


	// if --max-par=N than it remove functions wich has more than N parameters
	// insert the PARAMETERS COUNT NUMBER to structure
	foreach ($diced_function_array as $key => $structure) {
		if (preg_match("/(^[\s]*$|^[\s]*void[\s]*$|^[\s]*[\.][\.][\.][\s]*$)/um", $structure[5])==1) {
			$count=0; // function has ZERO parameters
			array_push($diced_function_array[$key], "0");
		}
		else {
			if (preg_match("/^[^,]*$/um", $structure[5])==1) {
				$count=1; // function has ONE parameter
				array_push($diced_function_array[$key], "1");
			}
			else {
				$count=preg_match_all("/[,]/u", $structure[5]); // function has TWO OR MORE parameters
				$count++;
				if (preg_match("/[,]\s*[\.][\.][\.]\s*$/uU", $structure[5])) { // "..." does not count as paramater
					$count--;
				}
				array_push($diced_function_array[$key], "$count");
			}
		}
		if ($flag_m) {
			if ($count > $mp_N) {
				unset($diced_function_array[$key]); // remove unwanted functions
			}
		}
	}

	/* depending on count of parameters saved in $structure[7] will do
	 	if 0 => $structure[8] = ""
	 	if 1 => $structure[8] = "type" of the only one parameter
	 	if 2 or more => $structure[8] is array of "type-s" of parameters
	 */
	foreach ($diced_function_array as $key => $structure) {
		if ($structure[7]=="0") {
			array_push($diced_function_array[$key], "");
		}
		else if ($structure[7]=="1") {
			array_push($diced_function_array[$key], trim(preg_replace("/[_a-zA-Z][_a-zA-Z0-9]*[^_a-zA-Z0-9]*$/u", "",$structure[5])));
		}
		else {
			$array=array();
			$array=explode(',', $structure[5]);
			foreach ($array as $idx => $value) {
				$array[$idx]=trim(preg_replace("/[_a-zA-Z][_a-zA-Z0-9]*[^_a-zA-Z0-9]*$/u", "", $value));
			}
			array_push($diced_function_array[$key], $array);
		}

	}

	// =================================================================================================
	// XML variables (preparing the xml printing)
	// =================================================================================================

	$xml_file;
	$xml_function_name;
	$xml_function_varargs;
	$xml_function_rettype;
	$xml_param_numer;
	$xml_param_type;

	if(empty($diced_function_array)) return; // file does not include any function, noting to print

	// foreach (array of functions as INDEX => FUNCTION)
	foreach ($diced_function_array as $key => $structure) { // structure include function data

		// file
			if (!($flag_i)) { // --input= wasn't given
				$xml_file=$file;
			}
			else { // --input= was given
				if (is_dir($input_FoD)) { // dir was given
					if (preg_match("/^.*[\/]$/um", $input_FoD)==1) {
						$path_to_remove=$input_FoD;
					}
					else {
						$path_to_remove=$input_FoD."/";
					}
					$xml_file=str_replace($path_to_remove, "", $file);
				}
				else { // file was given
					$xml_file=$input_FoD;
				}
			}

		// name
			$xml_function_name = trim($structure[4]);

		// varargs
			$xml_function_varargs = trim($structure[6]);

		// rettype
			if ($flag_r) {
				$xml_function_rettype = trim(preg_replace("/[\s]+/u", " ", $structure[1]));
				$xml_function_rettype = preg_replace("/[\s][\*]/u", "*", $xml_function_rettype);
			}
			else {
				$xml_function_rettype = $structure[1];
			}

		// print out function element
		// <function file="xml_file", name="xml_function_name", varargs="xml_function_varargs", rettype="xml_function_name">
			$xmlWriter->startElement("function");
			$xmlWriter->writeAttribute("file", trim($xml_file));
			$xmlWriter->writeAttribute("name", trim($xml_function_name));
			$xmlWriter->writeAttribute("varargs", trim($xml_function_varargs));
			$xmlWriter->writeAttribute("rettype", trim($xml_function_rettype));

		// param
			// no parameter
			if ($structure[7]=="0") {
				$xmlWriter->writeRaw("");
				$xmlWriter->endElement(); // end function </function>
				continue;
			}
			// one parameter
			else if ($structure[7]=="1") {

				if ($structure[8]==="...") { // if ... we do not have to print it out as param
					$xmlWriter->writeRaw("");
					$xmlWriter->endElement(); // end function </function>
					continue;
				}
				if (strlen(trim($structure[8])) == 0) { // if string include only whitespace (function had 1 param void => $structure[8] does not include character)
					$xmlWriter->writeRaw("");
					$xmlWriter->endElement(); // end function </function>
					continue;
				}

				$xml_param_numer = $structure[7];
				$xml_param_type = $structure[8];

				if ($flag_r) {
					$xml_param_type=preg_replace("/[\s]+/u", " ", $xml_param_type);
					$xml_param_type=preg_replace("/[\s]+[\*]/u", "*", $xml_param_type);
					$xml_param_type=preg_replace("/[\*][\s]+/u", "*", $xml_param_type);
				}

				$xmlWriter->startElement("param");
				$xmlWriter->writeAttribute("number", trim($xml_param_numer));
				$xmlWriter->writeAttribute("type", trim($xml_param_type));
				$xmlWriter->endElement(); // end param </param>
				$xmlWriter->endElement(); // end function </function>
			}
			// two or more parameters
			else {
				foreach ($structure[8] as $idx => $value) {
					if ($value==="...") continue; // if ... we do not have to print it out as param
					if (strlen(trim($value)) == 0) continue; // if string include only whitespace
					$xml_param_numer=$idx+1;
					$xml_param_type=$value;
					if ($flag_r) {
						$xml_param_type=preg_replace("/[\s]+/u", " ", $xml_param_type);
						$xml_param_type=preg_replace("/[\s]+[\*]/u", "*", $xml_param_type);
						$xml_param_type=preg_replace("/[\*][\s]+/u", "*", $xml_param_type);
					}
					$xmlWriter->startElement("param");
					$xmlWriter->writeAttribute("number", trim($xml_param_numer));
					$xmlWriter->writeAttribute("type", trim($xml_param_type));
					$xmlWriter->endElement(); // end param </param>
				}
				$xmlWriter->endElement(); // end function </function>
			}

	}
}

/*
	function do:
	- load file into string
	- remove useless data from string
	- analyze the rest of content (analyze() writes the xml output)
 */
function scanfile($file) {
	if (is_readable($file)==false) {
		error("Access denied for load data from file!", 2);
	}
	$content=file_get_contents($file);
	$string_formated=erase_rubbish($content);
	analyze($string_formated, $file);
}

/*
	function find .h files if directory is entered or if script is launched without parameter input
	if file is entered than nothing is provided
 */
function finder($in, $isdir) {

	global $flag_i;

	if ($isdir) {
		// SOURCE: http://stackoverflow.com/questions/15054997/find-all-php-files-in-folder-recursively
		// i adjust it to match my requirements
		$di = new RecursiveDirectoryIterator($in,RecursiveDirectoryIterator::SKIP_DOTS);
		$it = new RecursiveIteratorIterator($di);
		foreach($it as $file) {
			if (pathinfo($file, PATHINFO_EXTENSION) == "h") { // does it end with .h ?
				if ($flag_i) {
					scanfile($file);
				}
				else {
					$file_modified=str_replace(getcwd()."/", '', $file); // remove the unwanted path
					scanfile($file_modified);
				}
			}
		}
	}
	else {
		scanfile($in);
	}

}

function start_document() {
	global $flag_p, $flag_i;
	global $input_FoD, $px_K;
	global $xmlWriter;

	// setup indent and begin of document ===================
	// < ? xml version="1.0" encoding="UTF-8" ? >
	if ($flag_p) {
		$xmlWriter->startDocument('1.0', 'UTF-8');
		$xmlWriter->setIndent(true);
		$space="";
		for ($i=0; $i<$px_K; $i++) {
			$space=$space." ";
		}
		$xmlWriter->setIndentString("$space");
	}
	else {
		$xmlWriter->writeRaw("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	}

	// setup dir for <function dir"">
	if (!($flag_i)) { // --input= wasn't given
		$xml_dir="./";
	}
	else { // --input= was given
		if (is_dir($input_FoD)) { // dir was given
			$xml_dir=$input_FoD;
			if (!(preg_match("/^.*[\/]$/um", $xml_dir)==1)) { // dir must end with "/"
				$xml_dir .= "/";
			}
		}
		else { // file was given
			$xml_dir="";
		}
	}
	// <functions dir="$xml_dir">
	$xmlWriter->startElement("functions");
	$xmlWriter->writeAttribute("dir",$xml_dir);
	return;
}

/*
	this function is the main of script, it is called as first, other functions are called in it
 */
function main(array $argv, $argc) {

	global $flag_i, $flag_o, $flag_p, $flag_ni, $flag_m, $flag_nd, $flag_r;
	global $input_FoD, $output_F, $px_K, $mp_N;
	global $output;
	global $xmlWriter;

	check_parameters($argv, $argc);

	// open output file =====================================
	if ($flag_o == true) {


		if ($output_F == "") { // filename is not entered
			error("Output file name is not specified!", 3);
		}


		if (file_exists($output_F)) { // filename is entered and it exists
			if (is_dir($output_F)==true) { // it is dir
				error("Output file cannot be a directory!",3);
			}
			else { // it is file
				$fl=fopen($output_F, "w"); // can we open it for write ?
				if ($fl==NULL) { // no
					error("No access to output file",3);
				}
				else { // yes
					fclose($fl);
					$bool=$xmlWriter->openURI($output_F);
				}
			}
		}


		else { // filename does not exists so we create the file
			$bool=$xmlWriter->openURI($output_F);
		}


		if ($bool==false) {
			error("Unable to create an output file!", 3);
		}


	}
	else {
		$xmlWriter->openURI("php://stdout");
	}

	// input examination ====================================
	$flag_dir=false;
	if ($flag_i == true) {
		if ($input_FoD == "") { // --input=(nothing)
			error("Input file or directory name (path) is not specified!", 2);
		}
		if (is_dir($input_FoD)) {
			$flag_dir=true;
			$input=$input_FoD;
			start_document();
			finder($input, $flag_dir);
		}
		else {
			if (is_file($input_FoD)) {
				if (is_readable($input_FoD)==false) {
					error("Input file is not readable!",2);
				}
				$input=$input_FoD;
				start_document();
				finder($input, $flag_dir);
			}
			else {
				error("File or directory does not exist!", 2);
			}
		}
	}
	else {
		$flag_dir=true;
		$input=__DIR__;
		start_document();
		finder($input, $flag_dir);
	}



	// end for <function dir"">
	// </functions>
	$xmlWriter->writeRaw("");
	$xmlWriter->endElement();



	// write to output ========================================
	$xmlWriter->flush();



	exit(0);
}

?>
