#!/usr/bin/perl

#CHA:xlukac05
#author:Michal Lukac, xlukac05@stud.fit.vutbr.cz
#date:19.2.2012
####################################################
use locale;
use Cwd;

# Variables
$G_DIR = "./";			# processed directory
$G_NO_WHITESP = 0;	# if --no-whitespace
$G_NO_DUPLI = 0;		# --no-duplicates
$G_INLINE = 0;			# --no-inline
$G_INDENT = 0;			# --pretty-xml=k
$G_INDENT_HEAD = 0;
$G_MAX_PAR = "no";	# --max-par=n
$G_MAX_PAR2 = 0;
@G_IFILES = ();			# input filenames
@G_PIFILES = ();		# printable files
@G_OFILE = "";			# output filename
$G_ARGCNT = @ARGV;
$G_FPARAM = "";
@G_ERRVAL = (1,2,3,4,100);
@G_ERRVALS = ("Bad format of script params.",
			"Problem with opening input-file.",
			"Problem with opening output-file.",
			"Bad format of input-file.",
			"Unexpected error.");

# This is just simple function for printing help.
sub printHelp
{
	print "Author: Michal Lukac, xlukac05 at stud.fit.vutbr.cz\n",
	"Description: This script does analysis of c header files\n",
	"or all possible files of some directories and subdirs to xml.\n",
	"file or stdout.",
	"Possible params:--input=fileordir\n",
	"                --output=file\n",
	"                --pretty-xml=k\n",
	"                --no-inline\n",
	"                --max-par=n\n",
	"                --no-duplicates\n",
	"                --remove-whitespaces\n",;
	exit 0;
}

# If error in script occurs, this function prints
# error message and return with appropriate code.
sub exitError
{
	print STDERR "$_[0]\n","Returning value: $_[1].\n";
	exit $_[1];
}

# Function process params and set some gl. variables.
sub procParams
{
	use Getopt::Long;
	Getopt::Long::Configure('no_auto_abbrev');
	
	# parse arguments
	my $indent = 100000;
	GetOptions("--help" => \$help,'--input:s' => \$inputfile,
	'--output=s' => \$G_OFILE,'--pretty-xml:4' => \$indent,
	'--no-inline' => \$G_INLINE,'--max-par=i' => \$G_MAX_PAR,
	'--no-duplicates' => \$G_NO_DUPLI,
	'--remove-whitespace' => \$G_NO_WHITESP,
	) or exitError($G_ERRVALS[0],$G_ERRVAL[0]);

	# process arguments, asign them to g.vars
	if($help and $G_ARGCNT == 1) {printHelp();}
	elsif($help) {exitError($G_ERRVALS[0],$G_ERRVAL[0])}

	if($inputfile) {procFile($inputfile);}
	else {procFile();}

	if($indent != 100000) {$G_INDENT = $indent; $indent = 1; $G_INDENT_HEAD = 1;}
	else {$indent = 0;}

	if(@ARGV) {exitError($G_ERRVALS[0],$G_ERRVAL[0])}
	if($G_MAX_PAR ne "no") {$G_MAX_PAR2 = 1;} else {$G_MAX_PAR2 = 0;}

	foreach $param ($inputfile,$G_OFILE,$G_INLINE,$G_MAX_PAR2,$G_NO_DUPLI,$G_NO_WHITESP,$indent){
		if($param) {$count++;}
	}
	if($count != $G_ARGCNT) {exitError($G_ERRVALS[0],$G_ERRVAL[0]);}
}

# Specify files to read or write.
sub procFile
{
	# if input file is 'file' or 'directory'
	if($_[0])
	{
		if(-f $_[0])
		{
			$G_DIR = "";
			@G_IFILES = ("$_[0]");
		}
		elsif(-d $_[0])
		{
			$G_DIR = $_[0];
			searchDirs($_[0]);
		}
		else {exitError($G_ERRVALS[1],$G_ERRVAL[1]);} 
	}
	else
	{
		$dir = cwd;
		searchDirs(cwd());
	}

	# if output file is specified
	if($G_OFILE)
	{
		open(STDOUT,">","$G_OFILE") or 
		exitError($G_ERRVALS[2],$G_ERRVAL[2]);
	}
}

# Search all .h files in directory and his subdirs, add
# them to the G_IFILES array.
sub searchDirs
{
	find(\&getFiles,$_[0]);
}

# Helpfunction for searchdirs
sub getFiles
{
	use File::Find; push @G_IFILES, $File::Find::name if( /\.h$/i );
}

# Delete commentaries, macros and strings in text,...
sub prepareText
{
	$text = $_[0];
	@text2 = ();
	$count = 0;

	$text =~ s/\)\s*\{/\);{/g;									# ){ => );{
	$text =~ s/\)\s*\,/\);/g;
	foreach $char (split //, $text)									# {....}
	{
		if("$char" eq "{") {;$count++;}
		elsif("$char" eq "}") {;$count--;}
		if($count == 0) {push(@text2,$char);}
	}
	$text = join('',@text2);

	$text =~ s/"[^"\\]*(\\.[^"\\]*)*"//g;							# "..."
	$text =~ s/\/\/[^\n]*//g;										# //....
	$text =~ s/#[^\n]*//g;											# #....
	$text =~ s/((?:\/\*(?:[^*]|(?:\*+[^*\/]))*\*+\/)|(?:\/\/.*))//g;# /*...*/
	$text =~ s/typedef[^;\{}]+;//g;									# typedef ... ;
	$text =~ s/\w+\s*[\w\s\=\*\[\]]*;//g;							# unsigned int prom;
	return $text;
}

# This functions find all functions in text
# and remove inline functions if G_INLINE param is true.
sub findFunctions
{
	my @funcs = ();
	my @functions1 = ();
	my @funcs = ("$_[0]" =~ /[\w\s]+[^}{;]*;/g);

	# if not inline functions are set
	if ($G_INLINE)
	{
		foreach $function1 (@funcs)
		{
			if(!(($function1 =~ m/\sinline\s/) or ($function1 =~ m/^inline\s/)))
			{push (@functions1, "$function1");}	
		}
	}
	else {return @funcs;}
	return @functions1;
}

# Get params from string like "(....);"
sub getParams
{
	# reverse function string
	my $param = reverse($_[0]);
	my $state = 0; my $count = 0; my @text = ();
	
	# go through function and find (...)
	foreach $char (split //, $param)
	{
		if($state == 0)
		{
			if("$char" eq ")") {$count++;$state=1;}
			push(@text,$char);
		}
		elsif($state == 1)
		{
			if("$char" eq ")") {$count++;}
			elsif("$char" eq "(") {$count--;}
			push(@text,$char);
			if($count == 0) {last;}
		}
	}
	$param = join('',@text);
	
	# reverse it back
	$param = reverse($param);
	$G_FPARAM = $param;

	# find params with regexs in (....)
	$param =~ s/,/,,/g;
	my @params1 = ("$param" =~ /\(\s*(\w+[^,]*),/g);
	my @params2 = ("$param" =~ /,\s*([^,]+),/g);
	my @params3 = ("$param" =~ /,\s*([^,]+)\)\s*;/g);
	my @params4 = ("$param" =~ /\(\s*(\w+[^,]*)\)\s*;/g);
	my @params5 = ("$param" =~ /\(\s*\)\s*;/g);

	#return them as new array
	return (@params1,@params2,@params3,@params4,@params5);
}

# This function return proper filename
sub getFileName
{
	my $filename = $_[0];
	if("$G_DIR" eq "") {$file = $filename;}
	elsif("$G_DIR" eq "./")
	{
		$sub = cwd;
		$filename =~ s/$sub\///;
		$file = $filename;
	}
	else {$filename =~ s/$G_DIR//; $file=$filename;}

	return $filename;
}

# Print whitespace for better format
sub printWSpaces
{
	for($k = 0; $k < $G_INDENT*$_[0]; $k++)
	{print " ";}
}

# Remove spaces from text
sub removeSpaces
{
	my $text = $_[0];
	# if it param --no-whitespace specified
	if($G_NO_WHITESP)
	{
		$text =~ s/\s/ /g;
		$text =~ s/\s+/ /g;
		$text =~ s/\s+\*\s/\*/g;
		$text =~ s/\s+\[/\[/g;
	}
	return $text;
}

# The main function
sub programm
{
	# printing xml header
	print "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	if($G_INDENT_HEAD) {print "\n"}
	print "<functions dir=\"$G_DIR\">";
	if($G_INDENT_HEAD) {print "\n"}

	# Iterate all files
	foreach $filename (@G_IFILES)
	{
		my @functions = (); my @text1 = ""; my $i = 1; my $file = "";
		open(FILE,"<","$filename") or
			exitError($G_ERRVALS[1],$G_ERRVAL[1]);
		
		# Initial parsing of text of file
		@text1 = <FILE>;
		$text = join('',@text1);
		$text = prepareText($text);
		@functions = findFunctions($text);
		
		# Get filename
		$file = getFileName($filename);
		my @functionsname = ();

		# Iterate all functions
		foreach $function (@functions)
		{
			$hasVarArgs = "no";			
			# Get params from function
			@params = getParams($function);
		
			# Has variable arguments?
			$paramcount = 0; @newparams = ();
			foreach $param (@params)
			{
				if($param =~ /\.\.\./ ) {$hasVarArgs = "yes";}
				$paramcount++;
				# remove char *(argv)[]
				$param =~ s/([\*\[\]]+\s*)\w+(\s*[\*\[\]]+)/\1\2/;
			}

			# If (void) param
			if($G_FPARAM =~ /\(\s*void\s*\)\s*;/) { $paramcount = 0;}
			elsif($G_FPARAM =~ /\(\s*\)\s*;/) {$paramcount = 0;}

			# if has (...)
			if($hasVarArgs eq "yes") {$paramcount--;}

			# Get function name
			$function =~ s/\Q$G_FPARAM\E//;
			$funname = reverse($function); my $state = 0;
			@var = ($funname =~ /\w+/g);
			$funname = reverse(@var[0]);

			# Duplications
			my $isdupli = 0;
			if($G_NO_DUPLI)
			{
				foreach $funcn (@functionsname)
				{
					if ($funcn eq $funname){$isdupli = 1;}
				}
				push(@functionsname,$funname);
			}

			# Get rettype
			$function =~ s/\Q$funname\E//;
			$function =~ s/^[^\w]*//g;
			$function =~ s/[^\w\*]*$//g;
			$function = removeSpaces($function);

			# if is not duplication flag set
			if(!$isdupli)
			{
				# Final params processing
				my $numb = 1;
				if(($G_MAX_PAR >= $paramcount) or ($G_MAX_PAR2 == 0))
				{
					printWSpaces(1);
					print "<function file=\"$file\" name=\"$funname\" varargs=\"$hasVarArgs\" rettype=\"$function\">";
					if($G_INDENT_HEAD) {print "\n";}		
					if($paramcount)
					{
						# iterate all params and remmove unwanted characters
						foreach $param (@params)
						{
							if(!($param =~ /\.\.\./))
							{
								$param =~ s/^[^\w\[\]]*//g;
								$param =~ s/[^\w\[\]]*$//g;
								$param =~ s/\s*\w+$//g;
								$param = removeSpaces($param);
								
								printWSpaces(2);
								print "<param number=\"$numb\" type=\"$param\" />";
								if($G_INDENT_HEAD) {print "\n";}
								$numb++;
							}
						}
					}
					printWSpaces(1);
					print "</function>";
					if($G_INDENT_HEAD) {print "\n";}
				}
			}
		}
		close(FILE) or exitError($G_ERRVALS[1],$G_ERRVAL[1]);
	}
	# print end tag
	print "</functions>\n";
	#if($G_INDENT_HEAD) {print "\n";}
}

# Let The Program begin.
procParams();
programm();
close(STDOUT) or exitError($G_ERRVALS[2],$G_ERRVALS[2]);
exit 0;
