#!/usr/bin/python3
#-*- coding: utf-8 -*-
#XQR:xlukac05
#author:Michal Lukac, xlukac05@stud.fit.vutbr.cz
#date:3.2.2012
####################################################

import sys
import re
from xml.dom import minidom, Node

# params of program, select, from elements,....
G_MYDICT = {
	'order' : 0,
	'order_element' : "",
	'ifile' : "",
	'limit' : -1,
	'where' : [],
	'iswhere': 0,
	'from': "",
	'select': "",
	'tokens': [],
	'tokenindex' : 0,
	'genxml': True,
	'root': "",
	'query': "",
	'wherein': 0,}

# Errvalues connected with error messages
G_ERRVAL = (1,2,3,4,80,100);
G_ERRVALS = ("Bad format of script params.",
			"Problem with opening input-file.",
			"Problem with opening output-file.",
			"Bad format of input-file.",
			"Syntax/Semantics error",
			"Unexpected error.");

# Keywords of our simple 'sql' languague
G_KEYWORDS = ["SELECT", "FROM","WHERE","LIMIT","ORDER"];

# precedence table for precedence analysis
# A = AND, O = OR, N = NOT, OP = OPERATOR,
# EL = element, $ = start/end
#              A   O   N   (   )   OP  EL  $
PRE_TABLE = [['>','>','<','<','>','<','<','>'],
             ['<','>','<','<','>','<','<','>'],
             ['>','>','<','<','>','<','<','>'],
             ['<','<','<','<','>','<','<','X'],
             ['>','>','>','X','>','X','X','>'],
             ['>','>','X','<','>','X','<','>'],
             ['>','>','>','X','>','<','X','>'],
             ['<','<','<','<','X','<','<','X']];

# Help message
G_HMSG="""Author: Michal Lukac, xlukac05 at stud.fit.vutbr.cz
Description: This script evaluate sql query(similiar to sql SELECT query)
over the xml file. This query has simple conditions and syntax.
Possible params:--input=filename
                --output=filename
                --query=\'dotaz\'
                -n
                --root=element
                --qf=filename"""

# Item for precedence analysis, this item is 
# shift/reduct to/from stack
class ItemTable:
    def __init__(self, token, number):
        self.token, self.number = token, number;

# This is just simple function for printing help.
def printHelp():
	global G_HMSG
	if(len(sys.argv) != 2):
		exitError(G_ERRVALS[0],G_ERRVAL[0])
	print(G_HMSG)
	exit(0)

# This function prints error message and exit program.
def exitError(emesage,erval):
	print(emesage, file=sys.stderr)
	print("Returning value:"+str(erval),file=sys.stderr)
	exit(erval)

# This function get param after '='
def getParOfPar(param):
	array = re.split('=',param)
	return array[1]

# This function return text of opened file
def openReadFile(filename):
	global G_ERRVALS, G_ERRVAL
	try:
		filee = open(filename, "r")#, 1, "utf-8");
		text = filee.read()
		filee.close()
	except:
		exitError(G_ERRVALS[1],G_ERRVAL[1])
	return text;

# This function process params and set some g. vars
def procParams():
	global G_MYDICT, G_ERRVAL, G_ERRVALS
	p1 = 0; p2 = 0; p3 = 0; p4 = 0; p5 = 0; p6 = 0;
	query = ""; queryf = ""; ifile = ""; ofile = "";
	for i in range(len(sys.argv)):
		if sys.argv[i] == '-n':
			G_MYDICT['genxml'] = False; p1 = 1;
		elif '--help' in sys.argv[i]:
			printHelp()
		elif '--input=' in sys.argv[i]:
			ifile = getParOfPar(sys.argv[i]); p2 = 1
		elif '--output=' in sys.argv[i]:
			ofile = getParOfPar(sys.argv[i]); p3 = 1
		elif '--root=' in sys.argv[i]:
			G_MYDICT['root'] = getParOfPar(sys.argv[i]); p4 = 1
		elif '--qf=' in sys.argv[i]:
			queryf = getParOfPar(sys.argv[i]); p5 = 1
		elif '--query=' in sys.argv[i]:
			query = getParOfPar(sys.argv[i]); p6 = 1
		elif i != 0:
			exitError(G_ERRVALS[0],G_ERRVAL[0]);
	
	if(query and queryf):
		exitError(G_ERRVALS[0],G_ERRVAL[0])
	elif query:
		G_MYDICT['query'] = query
	elif queryf:
		G_MYDICT['query'] = openReadFile(queryf)
	else:
		exitError(G_ERRVALS[0],G_ERRVAL[0])
	if(ofile):
		sys.stdout = open(ofile,'w')
	if(ifile):
		sys.stdin = open(ifile,'r')
	if(not((p1+p2+p3+p4+p5+p6) == (len(sys.argv) -1))):
		exitError(G_ERRVALS[0],G_ERRVAL[0])
	G_MYDICT['ifile'] = sys.stdin.read()

# prepare text for lex analysis/searchtokens
def prepareText():
	global G_MYDICT
	G_MYDICT['query'] = re.sub("\(", " ( ", G_MYDICT['query']);
	G_MYDICT['query'] = re.sub("\)", " ) ", G_MYDICT['query']);
	G_MYDICT['query'] = re.sub("\<", " < ", G_MYDICT['query']);
	G_MYDICT['query'] = re.sub("\>", " > ", G_MYDICT['query']);
	G_MYDICT['query'] = re.sub("\=", " = ", G_MYDICT['query']);
	G_MYDICT['ifile'] = re.sub("<!--[^\!]+-->\s*", "", G_MYDICT['ifile']);

# get all tokens from text, simple lex analysis
def searchTokens():
	global G_MYDICT
	length = len(G_MYDICT['query']);
	state = 0; token = ""; tokens = [];

	# lex analysis
	for i in range(length):
		# basic state
		if(state == 0):
			token = "";
			if(G_MYDICT['query'][i].isspace()):
				pass;
			elif(G_MYDICT['query'][i] == "\""):
				state = 2;
				token = "\"";
			else:
				token = token + G_MYDICT['query'][i];
				state = 1;
		# if simple world
		elif(state == 1):
			if(G_MYDICT['query'][i].isspace()):
				state = 0;
				tokens.append(token);
			else:
				token = token+G_MYDICT['query'][i];
		# if string
		elif(state == 2):
			if(G_MYDICT['query'][i] == "\""):
				token = token + "\"";
				tokens.append(token);
				state = 0; token = "";
			else:
				token = token + G_MYDICT['query'][i];
		# if is end of string
		if((i == (length-1)) and G_MYDICT['query'][i].isspace()):
			token = "";
	tokens.append(token);
	for token in tokens:
		if(token != ""): G_MYDICT['tokens'].append(token);

# get token from tokenlist
def getToken():
	global G_MYDICT
	token = "";
	if((len(G_MYDICT['tokens'])-1) >= G_MYDICT['tokenindex']):
		token = G_MYDICT['tokens'][G_MYDICT['tokenindex']];
		G_MYDICT['tokenindex'] += 1;
	return token;

# if string is keyword return true else return false
def isKeyword(string):
	global G_KEYWORDS
	for item in G_KEYWORDS:
		if(string == item): return True;
	return False;

# do simple syntax analysis
def syntaxAnal():
	global G_MYDICT, G_ERRVALS, G_ERRVAL
	# first must be SELECT <element>
	if(getToken() != "SELECT"):	exitError(G_ERRVALS[4],G_ERRVAL[4]);
	G_MYDICT['select'] = getToken();
	if(G_MYDICT['select'] == ""): exitError(G_ERRVALS[4],G_ERRVAL[4]);

	# can be LIMIT <number>
	token = getToken(); islimit = 0;
	if(token == "LIMIT"):
		G_MYDICT['limit'] = getToken();
		if(G_MYDICT['limit'].isdigit()):
			G_MYDICT['limit'] = int(G_MYDICT['limit']);
		else:
			exitError(G_ERRVALS[4],G_ERRVAL[4]);
		islimit = 1;
	
	# must be FROM <element>
	if(islimit): token = getToken();
	if(token != "FROM"): exitError(G_ERRVALS[4],G_ERRVAL[4]);
	G_MYDICT['from'] = getToken();

	# can be WHERE ...
	token = getToken();
	if(token == "WHERE"):
		G_MYDICT['iswhere'] = 1;
		while(token != ""):
			token = getToken();
			if(token == "ORDER"):
				break;
			elif(token == ""):
				break;
			else:
				G_MYDICT['where'].append(token);
		G_MYDICT['where'].append("$");
	if(G_MYDICT['iswhere'] and (len(G_MYDICT['where']) == 0)):
		exitError(G_ERRVALS[4],G_ERRVAL[4]);

	# can be ORDER BY <element> [ASC|DESC]
	if(token == "ORDER"):
		token = getToken();
		if(token != "BY"): exitError(G_ERRVALS[4],G_ERRVAL[4]);
		token = getToken();
		if(token == ""): exitError(G_ERRVALS[4],G_ERRVAL[4]);
		G_MYDICT['order_element'] = token;
		token = getToken();
		if(token == "ASC"): G_MYDICT['order'] = 1;
		elif(token == "DESC"): G_MYDICT['order'] = 2;
		else: exitError(G_ERRVALS[4],G_ERRVAL[4]);

# return index of token to precedence table
def mapToTable(item):
	if(item == "AND"):
		return 0;
	elif(item == "OR"):
		return 1;
	elif(item == "NOT"):
		return 2;
	elif(item == "("):
		return 3;
	elif(item == ")"):
		return 4;
	elif(item == "<"):
		return 5;
	elif(item == ">"):
		return 5;
	elif(item == "="):
		return 5;
	elif(item == "CONTAINS"):
		return 5;
	elif(item == "$"):
		return 7;
	else:
		return 6;

# get token after WHERE
def getWhereToken():
	global G_MYDICT
	token = G_MYDICT['where'][G_MYDICT['wherein']];
	G_MYDICT['wherein'] += 1;
	return token;
##################################################################################################
def printstack(stack):
	print("*Stack*");
	for item in stack:
		print(item.number);
	print("*-----*");

# solve WHERE statement with precedence analysis
def whereSolve(node):
	global G_MYDICT, PRE_TABLE
	# init stack for precedence analysis
	stack = [];
	stack.append(ItemTable("$",7));
	token = getWhereToken();

	# go through all tokens from where
	while(1):
		# process token
		if(isKeyword(token)): exitError(G_ERRVALS[4],G_ERRVAL[4]);
		innumber = mapToTable(token);
		itemik = ItemTable(token,innumber);

		# last item on stack
		temp = stack[len(stack)-1];
		onstack = mapToTable(temp.token);
		result = PRE_TABLE[onstack][innumber];
		#printstack(stack);

		# shift item to stack
		if(result == "<"):
			stack.append(itemik);
			token = getWhereToken();
		# do reduction
		elif(result == ">"):
			# get last three items
			if(len(stack) >= 3):
				item1 = stack[len(stack)-3];
			item2 = stack[len(stack)-2];
			item3 = stack[len(stack)-1];
			inbrackets = 0;

			# if at input is $ or operator
			if (itemik.number == 7 or itemik.number == 1 or itemik.number == 2 or itemik.number == 0):
				isitbracket = 1;
			else:
				isitbracket = 0;
			
			# if is last token ')' and input is $/Operator and first item is and/or/not
			if(item3.token == ")" and isitbracket and (item1.number == 0 or item1.number == 1 or item1.number == 2)):
				inbrackets = 1;
				item1 = stack[len(stack)-4];
				item2 = stack[len(stack)-3];
				item3 = stack[len(stack)-2];
			
			# if in middle is operator <,=,>, CONTAINS
			typeo = 0; empty = 0;res = "";
			if(item2.number == 5):
				# if .attribute
				if(item1.token[0] == '.'):
					typeo = 1;
					for nodex in goTroughXml(node):
						if(nodex.hasAttribute(item1.token[1:])):
							res = nodex.attributes[item1.token[1:]].value;
							break;
				# if element.attribute
				elif('.' in item1.token):
					typeo = 2;
					ele = item1.token[:item1.token.index('.')];
					atr = item1.token[item1.token.index('.')+1:];temp3 = 1;
					if(node.tagName == ele):
						if(node.hasAttribute(atr)):
							res = node.getAttribute(atr);
							temp3 = 0;
					if(temp3):
						for nodex in goTroughXml(node):
							if(nodex.tagName == ele):
								if(nodex.hasAttribute(atr)):
									res = nodex.getAttribute(atr);
									break;
				# if element
				else:
					typeo = 3;
					nodes = node.getElementsByTagName(item1.token);
					if(item1.token == G_MYDICT['select']):
						typeo = 4;
						nodes.append(node);
					if(not nodes): empty = 1;
					else:
						res = nodes[0].toxml();
						res = res[(res.index('>')+1):(res.rindex('<'))];
					
			# if is less operation
			if(empty):
				resultik = False;
			elif(item2.token == "<"):
				if(item3.token.isdigit()):
					cmp1 = float(res);
					cmp2 = float(item3.token);
					resultik = (cmp1 < cmp2);
				else:
					resultik = (res < item3.token[1:-1]);
			# if is greater operation
			elif(item2.token == ">"):
				if(item3.token.isdigit()):
					cmp1 = float(res);
					cmp2 = float(item3.token);
					resultik = (cmp1 > cmp2);
				else:
					resultik = (res > item3.token[1:-1]);
			# if is equals operation
			elif(item2.token == "="):
				if(item3.token.isdigit()):
					cmp1 = float(res);
					cmp2 = float(item3.token);
					resultik = (cmp1 == cmp2);
				else:
					resultik = (res == item3.token[1:-1]);
			# if is CONTAINS operation
			elif(item2.token == "CONTAINS"):
				if("\"" not in item3.token):
					exitError(G_ERRVALS[4],G_ERRVAL[4]);						
				if(typeo == 1):
					if(item3.token[1:-1] in res):
						resultik = True;
					else:
						resultik = False;
				elif(typeo == 2):
					if(item3.token[1:-1] in res):
						resultik = True;
					else:
						resultik = False;
				elif(typeo == 3):
					if(not nodes[0].hasChildNodes()): resultik = False;
					elif(nodes[0].firstChild.nodeType == Node.TEXT_NODE):
						if(item3.token[1:-1] in res):
							resultik = True;
						else:
							resultik = False;
					else:
						G_MYDICT['wherein'] = 0;
						return False;
				elif(typeo == 4):
					if(item3.token[1:-1] in nodes[0].toxml()):
						resultik = True;
					else:
						resultik = False;
			# if is AND operation
			elif(item2.token == "AND"):
				if(item1.token and item3.token):
					resultik = True;
				else:
					resultik = False;
			# if is OR operation
			elif(item2.token == "OR"):
				if(item1.token or item3.token):
					resultik = True;
				else:
					resultik = False;
			# if is NOT operation
			elif(item2.token == "NOT"):
				resultik = not item3.token;
			# if is two brackets
			elif(item1.token == "(" and item3.token == ")"):
				resultik = item2.token;
			# if is end of precedence analysis
			elif(item2.token == "$" and itemik.token == "$"):
				G_MYDICT['wherein'] = 0;
				return item3.token;
			else:
				print("UNEXPECTED ERROR");
				exit(erval);
            
			newresult = ItemTable(resultik,6);
			if(inbrackets):
				if(item2.token != "NOT"):
					stack.pop(len(stack)-2);
				stack.pop(len(stack)-2);
				stack.pop(len(stack)-2);
				stack.insert(len(stack)-1,newresult);
				pass;
			else:
				if(item2.token != "NOT"):
					if(len(stack) >= 2):
						stack.pop();
				if(len(stack) >= 2):
					stack.pop();
				if(len(stack) >= 2):
					stack.pop();
				stack.append(newresult);
			
			if((itemik.number == 1 or itemik.number == 2 or itemik.number == 0) and stack[len(stack)-1].number == 4):
				pass;
			elif(itemik.token == "$"):
				pass;
			elif(stack[len(stack)-2].number == 2):
				pass;
			else:
				stack.append(itemik);
				token = getWhereToken();
		# else is syntax/semantics error
		else:
			exitError(G_ERRVALS[4],G_ERRVAL[4]);
	return False;
	pass;

# source for this function:
# http://www.rexx.com/~dkuhlman/pyxmlfaq.html
def goTroughXml(node):
	if node.nodeType == Node.ELEMENT_NODE:
		yield node;
		for child in node.childNodes:
			for n1 in goTroughXml(child):
				yield n1;

# parse xml, print output, call precedence analysis
def parseXML():
	global G_MYDICT
	fromnodes = [];
	xmldoc = minidom.parseString(G_MYDICT['ifile']);

	# if ROOT search from root node
	if(G_MYDICT['from'] == "ROOT"):
		fromnodes = xmldoc.childNodes;
	else:
		# if .attribute
		if(G_MYDICT['from'][0] == '.'):
			rootnode = xmldoc.documentElement;
			for node in goTroughXml(rootnode):
				if(node.hasAttribute(G_MYDICT['from'][G_MYDICT['from'].index('.')+1:])):
					fromnodes.append(node);
		# if element.attribute
		elif('.' in G_MYDICT['from']):
			nodes = xmldoc.getElementsByTagName(G_MYDICT['from'][:G_MYDICT['from'].index('.')]);
			for node in nodes:
				if node.hasAttribute(G_MYDICT['from'][G_MYDICT['from'].index('.')+1:]):
					fromnodes.append(node);
		# if element
		else:
			fromnodes = xmldoc.getElementsByTagName(G_MYDICT['from']);
	
	
	truestory = False;
	# iterate all nodes from FROM
	for node1 in fromnodes:
		selectnodes = node1.getElementsByTagName(G_MYDICT['select']);
		i = 0; listtoprint = [];
		# iterate all nodes from SELECT->FROM
		for node2 in selectnodes:
			if(G_MYDICT['limit'] == i):
				break;
			# do precedence analysis
			if(G_MYDICT['iswhere']): ok = whereSolve(node2);
			else: ok = True;
			if(ok):
				truestory = True;
				listtoprint.append(node2);
			if(G_MYDICT['order']): pass;
			i += 1;
		# write result
		if(truestory):
			# if generate xml header
			if(G_MYDICT['genxml'] and G_MYDICT['limit']!= 0):
				print("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
			# if print root element
			if(G_MYDICT['root']):
				print("<"+G_MYDICT['root']+">");
			for item in listtoprint:
				print(item.toxml());
			if(G_MYDICT['root']):
				print("</"+G_MYDICT['root']+">");
			return;
		else:
			if(not listtoprint):
				print("<EmptyRoot/>\n");
				return;

# main function
def programm():
	prepareText();
	searchTokens();
	syntaxAnal();
	parseXML();

# Let the program begin
procParams();
programm();