#!/usr/bin/env python3.6

# Principles of Programming Languages - IPP
# Project 2 - XQR (XML Query)
# Adrian Toth - xtotha01
# 4.4.2017

#########################################################################################################################
# IMPORTS

import sys    # basic [stdin,stdout,stderr,exit] stuff
import codecs # input file handling in In() function
import re     # using regex match, compile
import xml.dom.minidom as xdm # reading data from xml

#########################################################################################################################
# FUNCTIONS

# is integer
def isInt(s):
	try:
		int(s)
		return True
	except ValueError:
		return False

# is float
def isFloat(s):
	try:
		float(s)
		return True
	except ValueError:
		return False

# is string
def isString(s):
	return True if isinstance(s, str) else False

# function which prints out input message and terminate the script with input exitcode
def Error(message,exitcode):
	sys.stderr.write(message)
	sys.exit(exitcode)

# print help and exit
def Help():
	message= "\nScript's parameters:\n\n" + \
			 "\tInput destination:\n" + \
			 "\t[--input=name] where name represents file or directory\n\n" + \
			 "\tOutput destination:\n" + \
			 "\t[--output=name] where name represents file\n\n" + \
			 "\tInput select:\n" + \
			 "\t[--query='select'] where select has defined format in task, it does not include apostrophe\n\n" + \
			 "\tInput query in file:\n" + \
			 "\t[--qf=filename] filename is a file where is select saved, this param cannot be combined with \"--query=\'select\'\"\n\n" + \
			 "\tNo xml header:\n" + \
			 "\t[-n]\n\n" + \
			 "\tName of root element:\n" + \
			 "\t[--root=element] where element include results\n\n" + \
			 "\tHelp:\n" + \
			 "\t[--help]\n\n"
	sys.stdout.write(message)
	sys.exit(0)

# Argument checker (on the basis of it returns a list which is filled with values depending on type of argument)
def isArg(str):
	if   re.match("^--input=.*$",  str): return (True,"input" ,str[8:])
	elif re.match("^--output=.*$", str): return (True,"output",str[9:])
	elif re.match("^--query=.*$",  str): return (True,"query" ,str[8:])
	elif re.match("^--qf=.*$",     str): return (True,"qf"    ,str[5:])
	elif re.match("^--root=.*$",   str): return (True,"root"  ,str[7:])
	elif re.match("^-n$",          str): return (True,"n"     ,True)
	elif re.match("^--help$",      str): return (True,"help"  ,True)
	else: return (False,"","")

# setup input
def In(args):
	if (args.input != None): # read inputfile
		if (args.input == ""): Error("Wrong input file!\nFilename after option --input is not specified.\n",2)
		else:
			try:
				sys.stdin=codecs.open(args.input, "r", "utf-8")
			except:
				Error("Input file \""+args.input+"\" cannot be open for reading!\n",2)
	else: # read stdin
		return

# setup output
def Out(args):
	if (args.output != None): # write to outputfile
		if (args.output == ""): Error("Wrong output file!\nFilename after option --output is not specified.\n",3)
		else:
			try:
				sys.stdout=open(args.output, "w")
			except:
				Error("Output file \""+args.output+"\" cannot be open for writing!\n",3)
	else: # write to stdout
		return

# setup select
def Sql(args):
	if (args.qf != None): # load select from file
		if (args.qf == ""):
			Error("Wrong query file!\nFilename after option --qf is not specified.\n",80)
		else:
			try:
				with open(args.qf, "r") as content_file:
					sel=content_file.read()
			except:
				Error("Cannot read file \""+args.qf+"\"!\n",80)
	else: # load select value from --query=value
		sel=args.query
	return sel

####################################
# SCANNER FUNCTIONS

# scanner function, input=string, output=list(tokens)
def tokenize(string):
	buff=""
	string += " }~" # end "}" is for END TOKEN, tilde "~" represents end of string for scanner
	tokens=list()
	for c in string:
		if (not c.isspace() and c != "~"): # read one value for one token
			buff += c
		else:
			if not buff: continue
			tokens.append(evaluate(buff)) # determine the loaded value type and insert the token to the list
			buff=""
	return tokens # return list of tokens

# return appertaining Token(id,value) depending on string, (d=digir, c=char)
def evaluate(s):
	#print("LEXEM:",s) # DEBUG
	ele = re.compile("^[_:a-zA-Z][_:a-zA-Z0-9-]*$")
	dotatt = re.compile("^\.[_:a-zA-Z][_:a-zA-Z0-9-]*$")
	eledotatt = re.compile("^[_:a-zA-Z][_:a-zA-Z0-9-]*\.[_:a-zA-Z][_:a-zA-Z0-9-]*$")
	# KEYWORD
	if   (s == "SELECT"  ): return Token("KEYWORD",s) # SELECT
	elif (s == "FROM"    ): return Token("KEYWORD",s) # FROM
	elif (s == "WHERE"   ): return Token("KEYWORD",s) # WHERE
	elif (s == "LIMIT"   ): return Token("KEYWORD",s) # LIMIT
	elif (s == "NOT"     ): return Token("KEYWORD",s) # NOT
	elif (s == "CONTAINS"): return Token("KEYWORD",s) # CONTAINS
	# RELATION-OPERATOR
	elif (s == ">"): return Token("GREATER",s) # >
	elif (s == "<"): return Token("LESS",s)    # <
	elif (s == "="): return Token("EQUAL",s)   # =
	# NUMBER
	elif (isInt(s)): return Token("INT",int(s)) # (d) or (+d) or (-d)
	# STRING
	elif (s.startswith('"') and s.endswith('"')): return Token("STRING",s) # "string"
	# ELEMENT-OR-ATTRIBUTE
	elif (ele.match(s)): return Token("ELEMENT",s)                 # c
	elif (dotatt.match(s)): return Token(".ATTRIBUTE",s)           # .c
	elif (eledotatt.match(s)): return Token("ELEMENT.ATTRIBUTE",s) # c.c
	# $
	elif (s == "}"): return Token("END","$") # END TOKEN
	# ERROR
	else: Error("Lexical error!\nWrong lexeme \""+s+"\".\n",80) # NO MATCH

####################################
# PARSER FUNCTIONS

# prints out syntax error information what did evoke the error
def print_syntax_error(token):
	if not token: return
	if (token.type == "END"): sys.stderr.write("Select is incomplete!\n")
	else: sys.stderr.write("Unexpected \""+str(token.value)+"\"!\n")

# <QUERY>
def query(scanner):
	#print ("= query =") # DEBUG
	token=scanner.get_token()
	if (token and token.value == "SELECT"): # SELECT
		token=scanner.get_token()
		if (token and token.type == "ELEMENT"): # ELEMENT
			token=scanner.get_token()
			if (token and token.value == "FROM"): # FROM
				return from_elm(scanner) and where_clause(scanner) and order_clause(scanner) and limitn(scanner) and catch_end_token(scanner)
			else:
				print_syntax_error(token)
				return False
		else:
			print_syntax_error(token)
			return False
	else:
		print_syntax_error(token)
		return False

# <FROM-ELM>
def from_elm(scanner):
	#print ("= from_elm =") # DEBUG
	token=scanner.get_token()
	if (token and token.value == "ROOT"): # ROOT
		return True
	elif (token and (token.value == "WHERE" or token.value == "LIMIT" or token.type == "END")): # empty
		scanner.return_token()
		return True
	else: # <ELEMENT-OR-ATTRIBUTE>
		scanner.return_token()
		return element_or_attribute(scanner)

# <WHERE-CLAUSE>
def where_clause(scanner):
	#print ("= where_clause =") # DEBUG
	token=scanner.get_token()
	if (token and token.value == "WHERE"): # WHERE
		return condition(scanner)
	elif (token and (token.value == "LIMIT" or token.type == "END")): # empty
		scanner.return_token()
		return True
	else:
		print_syntax_error(token)
		return False

# <ORDER-CLAUSE>
def order_clause(scanner): # useless function for project which do not do additional enhancements
	#print ("= order_clause =") # DEBUG
	token=scanner.get_token()
	if (token and (token.value == "LIMIT" or token.type == "END")): # empty
		scanner.return_token()
		return True
	else:
		print_syntax_error(token)
		return False

# <LIMITn>
def limitn(scanner):
	#print ("= limitn =") # DEBUG
	token=scanner.get_token()
	if (token and token.value == "LIMIT"): # LIMIT
		token=scanner.get_token()
		if (token and token.type == "INT"): # number
			return True
		else:
			print_syntax_error(token)
			return False
	elif (token and token.type == "END"): # empty
		scanner.return_token()
		return True
	else:
		print_syntax_error(token)
		return False

# <CONDITION>
def condition(scanner):
	#print ("= condition =") # DEBUG
	token=scanner.get_token()
	if (token and token.value == "NOT"): # NOT
		return condition(scanner)
	else:
		scanner.return_token()
		return element_or_attribute(scanner) and relation_operator(scanner) and literal(scanner)

# <LITERAL>
def literal(scanner):
	#print ("= literal =") # DEBUG
	token=scanner.get_token()
	if (token and token.type == "STRING"): # string
		return True
	elif (token and token.type == "INT"): # number
		return True
	else:
		print_syntax_error(token)
		return False

# <RELATION-OPERATOR>
def relation_operator(scanner):
	#print ("= relation_operator =") # DEBUG
	token=scanner.get_token()
	if (token and token.value == "CONTAINS"): # CONTAINS
		return True
	elif (token and token.type == "EQUAL"): # =
		return True
	elif (token and token.type == "GREATER"): # >
		return True
	elif (token and token.type == "LESS"): # <
		return True
	else:
		print_syntax_error(token)
		return False

# <ELEMENT-OR-ATTRIBUTE>
def element_or_attribute(scanner):
	#print ("= element_or_attribute =") # DEBUG
	token=scanner.get_token()
	if (token and token.type == "ELEMENT"): # element
		return True
	elif (token and token.type == ".ATTRIBUTE"): # .attribute
		return True
	elif (token and token.type == "ELEMENT.ATTRIBUTE"): # element.attribute
		return True
	else:
		print_syntax_error(token)
		return False

# catch END token "$"
def catch_end_token(scanner):
	#print ("= catch_end_token =") # DEBUG
	token=scanner.get_token()
	if (token and token.type == "END"):
		return True
	else:
		print_syntax_error(token)
		return False

# the parser
def parser(scanner):
	if (query(scanner) == True): # syntactic analysis (calling parser's init function)
		return # syntactically alright
	else:
		Error("Syntactic error!\n",80)

####################################
# SELECT FUNCTIONS

# get element_or_attribute's type
def getType_EoA(string):
	if (string == None): return None

	ele = re.compile("^[^\.\ ]+$")
	dotatt = re.compile("^[\.][^\.\ ]+$")
	eledotatt = re.compile("^[^\.\ ]+[\.][^\.\ ]+$")

	if   ele.match(string):       return {"ELEMENT":string,"ATTRIBUTE":None}
	elif dotatt.match(string):    return {"ELEMENT":None  ,"ATTRIBUTE":string[1:]}
	elif eledotatt.match(string): return {"ELEMENT":string.rpartition('.')[0],"ATTRIBUTE":string.rpartition('.')[2]}
	else: return None

# evaluate condition (value(Elem-Or-Att) OPERATOR Literal)
def evaluate_condition(eoa,operator,literal):
	if (operator=="CONTAINS"):
		if (isInt(literal)): Error("Wrong query!\nAfter \"CONTAINS\" must be string!\n",80) # handled before this function
		else: return True if eoa.find(literal[1:-1])!=-1 else False
	elif (operator==">"):
		if isInt(literal) and isFloat(eoa): return float(eoa) >  int(literal)
		else: return eoa > literal[1:-1]
	elif (operator=="<"):
		if isInt(literal) and isFloat(eoa): return float(eoa) <  int(literal)
		else: return eoa < literal[1:-1]
	elif (operator=="="):
		if isInt(literal) and isFloat(eoa): return float(eoa) == int(literal)
		else: return eoa == literal[1:-1]
	#else: WILL NEVER HAPPEN

# perform the select which has correct format
def select_data(select):

	select = select.strip() # trim whitespaces on both side of select
	parts = {
				"SELECT"   :None,  # it will obtain value dict { "ELEMENT":value, "ATTRIBUTE":value }
				"FROM"     :None,  # it will obtain value dict { "ELEMENT":value, "ATTRIBUTE":value }
				"WHERE"    :None,  # it will obtain value list ( {"ELEMENT":value, "ATTRIBUTE":value}, "relation-operator", "string/number" ) depending on condition
				"NEGATION" :False, # it will obtain value bool True=NOT(condition), False=(condition)
				"LIMIT"    :None   # it will obtain int value
			}

	regex_str = "^SELECT\s+(\S+)\s+FROM\s*(?:(?:(\S+)\s*)?(?:(?:WHERE\s+(.*?))?(?:\s*LIMIT\s+(\S+)\s*)?)?)?$"
	regex = re.compile(regex_str)
	array = re.match(regex, select)

	parts["SELECT"] = getType_EoA(array.group(1)) # SELECT's value
	parts["FROM"  ] = getType_EoA(array.group(2)) # FROM's   value

	parts["WHERE"]  = array.group(3) # WHERE's  condition
	parts["LIMIT"]  = array.group(4) # LIMIT's  value


	if parts["WHERE"]: # set the NEGATION value depending on NOT in condition of WHERE
		count=0;
		for word in parts["WHERE"].split():
			if (word == "NOT"): count += 1
		if (count % 2 == 1): parts["NEGATION"] = True
		parts["WHERE"]=re.sub("(?:NOT\s+)+","",parts["WHERE"]) # remove NOT / NOTs, from now NOTs are useless
		parts["WHERE"]=(getType_EoA(parts["WHERE"].split()[0]),parts["WHERE"].split()[1],parts["WHERE"].split()[2])
		if (parts["WHERE"][1]=="CONTAINS" and isInt(parts["WHERE"][2])): Error("Wrong query!\nAfter \"CONTAINS\" must be string!\n",80) # CHECK beforehand evaluate_condition() function is called

	if parts["LIMIT"]:
		parts["LIMIT"] = int(parts["LIMIT"])
		if (parts["LIMIT"]<0):
			Error("Wrong query!\nAfter LIMIT cannot be negative number, number must be pozitive integer or zero.\n",80)

	#for item in parts: print(item,"   \t=",parts[item]) # DEBUG

	xml_reader = xdm.parse(sys.stdin)

	# FROM ##########################################################
	FROM = parts["FROM"] # value = { "ELEMENT":value, "ATTRIBUTE":value }
	from_data=[]

	# FROM empty
	if (FROM == None): return []

	# FROM element
	elif (FROM["ELEMENT"]!=None and FROM["ATTRIBUTE"]==None):

		# FROM ROOT
		if (FROM["ELEMENT"] == "ROOT"):
			from_data = [xml_reader.documentElement]

		# FROM element
		else:
			try: from_data = [xml_reader.getElementsByTagName(FROM["ELEMENT"])[0]]
			except: from_data = []

	# FROM .attribute
	elif (FROM["ELEMENT"]==None and FROM["ATTRIBUTE"]!=None):
		elements = xml_reader.getElementsByTagName("*")
		for e in elements:
			if e.hasAttribute(FROM["ATTRIBUTE"]):
				from_data=[e]
				break

	# FROM element.attribute
	elif (FROM["ELEMENT"]!=None and FROM["ATTRIBUTE"]!=None):
		elements = xml_reader.getElementsByTagName(FROM["ELEMENT"])
		for e in elements:
			if e.hasAttribute(FROM["ATTRIBUTE"]):
				from_data=[e]
				break

	if (len(from_data)==0):
		#Error("Sematic error!\nThe query does not select any data from xml.\nError at: FROM element-or-attribute\n",80)
		return []

	# DEBUG
	"""
	print("====================================\nFROM DATA")
	for fd in from_data:
		print("#FD#",fd.toxml())
	"""

	# SELECT ########################################################
	SELECT = parts["SELECT"] # value = { "ELEMENT":value, "ATTRIBUTE":value }
	select_data=[None]

	if (SELECT["ELEMENT"]=="ROOT" and FROM["ELEMENT"]=="ROOT"): # SELECT ROOT FROM ROOT
		select_data=[xml_reader.documentElement]
	else:
		select_data=[]
		for e in from_data:
			select_data.extend(e.getElementsByTagName(SELECT["ELEMENT"]))

	if (len(select_data)==0):
		#Error("Sematic error!\nThe query does not select any data from xml.\nError at: SELECT element\n",80)
		return []

	# DEBUG
	"""
	print("====================================\nSELECT DATA")
	for sd in select_data:
		print("#SD#",sd.toxml())
	"""

	# WHERE #########################################################
	data=[]
	if not parts["WHERE"]: data=select_data
	if parts["WHERE"]:
		WHERE  = parts["WHERE"]    # value = ( {"ELEMENT":value, "ATTRIBUTE":value}, "relation-operator", "string/number" )
		NEGATE = parts["NEGATION"] # value = True or False

		# element
		if (WHERE[0]["ELEMENT"]!=None and WHERE[0]["ATTRIBUTE"]==None):
			for e in select_data:
				# e = element <book>....</book> with all subelements (author, price, ...)
				for e2 in e.getElementsByTagName(WHERE[0]["ELEMENT"]):
					# e2 = elemet <author>...STRING...</author>

					for node in e2.childNodes:
						if node.nodeType == xdm.Node.TEXT_NODE:
							if not node.nodeValue.isspace():
								if (NEGATE==True):
									if (evaluate_condition(e2.firstChild.nodeValue, WHERE[1], WHERE[2]) == False): data.append(e)
								else:
									if (evaluate_condition(e2.firstChild.nodeValue, WHERE[1], WHERE[2]) == True ): data.append(e)
						else:
							Error("Element is not text node!\n",4)

		# attribute
		elif (WHERE[0]["ELEMENT"]==None and WHERE[0]["ATTRIBUTE"]!=None):
			for e in select_data:
				if (e.hasAttribute(WHERE[0]["ATTRIBUTE"])):
					if (NEGATE==True):
						if (evaluate_condition(e.getAttribute(WHERE[0]["ATTRIBUTE"]), WHERE[1], WHERE[2]) == False): data.append(e)
					else:
						if (evaluate_condition(e.getAttribute(WHERE[0]["ATTRIBUTE"]), WHERE[1], WHERE[2]) == True ): data.append(e)

		# element.attribute
		elif (WHERE[0]["ELEMENT"]!=None and WHERE[0]["ATTRIBUTE"]!=None):
			for e in select_data:
				if (e.tagName == WHERE[0]["ELEMENT"]):
					if (e.hasAttribute(WHERE[0]["ATTRIBUTE"])):
						if (NEGATE==True):
							if (evaluate_condition(e.getAttribute(WHERE[0]["ATTRIBUTE"]), WHERE[1], WHERE[2]) == False): data.append(e)
						else:
							if (evaluate_condition(e.getAttribute(WHERE[0]["ATTRIBUTE"]), WHERE[1], WHERE[2]) == True ): data.append(e)

	# LIMIT #########################################################
	if (parts["LIMIT"] or parts["LIMIT"]==0): data = data[:parts["LIMIT"]]

	return data

# print the selected data from xml
def print_data(data,args):
	if not args.n: sys.stdout.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>")
	if data:
		if args.root: sys.stdout.write("<"+args.root+">")
		for i in data:
			sys.stdout.write(re.sub("\n\s*\n","\n",i.toprettyxml()))
		if args.root: sys.stdout.write("</"+args.root+">")
	else:
		if args.root: sys.stdout.write("<"+args.root+"/>")

#########################################################################################################################
# CLASSES

# Class provide argument check and save them separately into class variables, argparse is not suitable (does not work as i want)
class Args():

	def __init__(self,args):

		self.args={
					  "input"  :{ "Set":False, "Value":None },
					  "output" :{ "Set":False, "Value":None },
					  "query"  :{ "Set":False, "Value":None },
					  "qf"     :{ "Set":False, "Value":None },
					  "n"      :{ "Set":False, "Value":None },
					  "root"   :{ "Set":False, "Value":None },
					  "help"   :{ "Set":False, "Value":None }
				  }

		for arg in args[1:]: # args[0] = script_name (useless for me)
			val = isArg(arg)
			if val[0]: # supported argument
				if not (self.args[val[1]])["Set"]: # there is no duplicity (for the first time we process this argument)
					self.args[val[1]]["Set"  ] = True
					self.args[val[1]]["Value"] = val[2]
				else: # duplicity (in past we processed this argument so we cannot process him again)
					Error("Wrong argument!\nDuplicit argument \""+arg+"\".\n",1)
			else: # unsupported argument
				Error("Wrong argument!\nArgument \""+arg+"\" is not supported.\n",1)

		# assign value to appertaining class variable
		Args.input  = self.args["input" ]["Value"]
		Args.output = self.args["output"]["Value"]
		Args.query  = self.args["query" ]["Value"]
		Args.qf     = self.args["qf"    ]["Value"]
		Args.n      = self.args["n"     ]["Value"]
		Args.root   = self.args["root"  ]["Value"]
		Args.help   = self.args["help"  ]["Value"]

# token representing a lexical unit (single word in string separated by whitespace)
class Token():

	def __init__(self, type, value):
		self.type=type
		self.value=value

	def type(self): # type of string
		return self.type

	def value(self): # string
		return self.value

# scanner, on init it makes and hold the token list, for access to the list of tokens there are called other methods
class Scanner():

	def __init__(self, string):
		self.tokens=tokenize(string)
		self.token_idx=0

	def get_token(self):
		#print("\t   GET",self.token_idx,self.tokens[self.token_idx].type,"==",self.tokens[self.token_idx].value) # DEBUG
		token=self.tokens[self.token_idx]
		self.token_idx += 1
		return token

	def return_token(self):
		self.token_idx -= 1
		#print("\tRETURN",self.token_idx,self.tokens[self.token_idx].type,"==",self.tokens[self.token_idx].value) # DEBUG

#########################################################################################################################
# MAIN BODY

####################################
# Setup variables depending from arguments

args=Args(sys.argv)

####################################
# Process --help

if args.help: # is --help
	if (len(sys.argv)==2): Help() # print Help
	else: Error("Wrong arguments!\nArgument --help cannot be combined with other argumets.\n",1) # is --help with some another argument/arguments

####################################
# Process --query and --qf

if ( (args.query == None) and (args.qf == None) ): # --query and --qf was not entered
	Error("Wrong parameters!\nParameter \"--query=\'select\'\" or \"--qf=filename\" was not entered.\n",80)
elif ( (args.query != None) and (args.qf != None) ): # --query and --qf was entered together
	Error("Wrong parameters!\nParameters \"--query=\'select\'\" or \"--qf=filename\" cannot be combined.\n",1)

####################################
# Process value of parameters

# input
In(args)

# output
Out(args)

# load select
select=Sql(args)

####################################
# Checking and executing the select


# BUGFIX!!! DONT TOUCH !!! IF YOU REMOVE THIS SCANNER (LEXICAL ANALYSATOR) WONT WORK !!! DANGER !!! WARNING !!!!
if ">" in select: select=re.sub(">", " > ",select) # BUGFIX
if "<" in select: select=re.sub("<", " < ",select) # BUGFIX
if "=" in select: select=re.sub("=", " = ",select) # BUGFIX
if " CONTAINS" in select: select=re.sub("CONTAINS", " CONTAINS ",select) # BUGFIX
# i had to do this bugfix because i did not count that the characters cannot be divided by whitespace, so my scanner works if i insert whitespace


# scanner (lexical check of select)
scanner=Scanner(select)

# parser (syntactic check of select), parser need to call for tokens from scanner
parser(scanner)

# get data via select
data=select_data(select)

# print data
print_data(data,args)

# THE END
sys.exit(0)
