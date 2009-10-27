######################################################################
# The remainder of this file is from parsedesc.{g,py}

def append(lst, x):
    "Imperative append"
    lst.append(x)
    return lst

def add_inline_token(tokens, str):
    tokens.insert( 0, (str, eval(str, {}, {})) )
    return Terminal(str)

def cleanup_choice(lst):
    if len(lst) == 0: return Sequence([])
    if len(lst) == 1: return lst[0]
    return apply(Choice, tuple(lst))

def cleanup_sequence(lst):
    if len(lst) == 1: return lst[0]
    return apply(Sequence, tuple(lst))

def cleanup_rep(node, rep):
    if rep == 'star':   return Star(node)
    elif rep == 'plus': return Plus(node)
    else:               return node

def resolve_name(tokens, id, args):
    if id in map(lambda x: x[0], tokens):
	# It's a token
	if args: 
	    print 'Warning: ignoring parameters on TOKEN %s<<%s>>' % (id, args)
        return Terminal(id)
    else:
        # It's a name, so assume it's a nonterminal
        return NonTerminal(id, args)

%%
parser ParserDescription:
    option:      "context-insensitive-scanner"

    ignore:      "[ \t\r\n]+"
    ignore:      "#.*?\r?\n"
    token END:   "$"
    token ATTR:  "<<.+?>>"
    token STMT:  "{{.+?}}"
    token ID:    '[a-zA-Z_][a-zA-Z_0-9]*'
    token STR:   '[rR]?\'([^\\n\'\\\\]|\\\\.)*\'|[rR]?"([^\\n"\\\\]|\\\\.)*"'
    token LP:    '\\('
    token RP:    '\\)'
    token LB:    '\\['
    token RB:    '\\]'
    token OR:    '[|]'
    token STAR:  '[*]'
    token PLUS:  '[+]'
    token QUEST: '[?]'
    token COLON: ':'

    rule Parser: "parser" ID ":"
                   Options
                   Tokens
                   Rules<<Tokens>> 
                 END 
                 {{ return Generator(ID,Options,Tokens,Rules) }}

    rule Options: {{ opt = {} }}
                  ( "option" ":" Str {{ opt[Str] = 1 }} )*
                  {{ return opt }}

    rule Tokens:  {{ tok = [] }}
                  (
                    "token" ID ":" Str {{ tok.append( (ID,Str) ) }}
                  | "ignore"   ":" Str {{ tok.append( ('#ignore',Str) ) }}
                  )*
                  {{ return tok }}

    rule Rules<<tokens>>:
                  {{ rul = [] }}
                  (
                    "rule" ID OptParam ":" ClauseA<<tokens>>
                    {{ rul.append( (ID,OptParam,ClauseA) ) }}
                  )*
                  {{ return rul }}

    rule ClauseA<<tokens>>:
                  ClauseB<<tokens>>
                  {{ v = [ClauseB] }}
                  ( OR ClauseB<<tokens>> {{ v.append(ClauseB) }} )*
                  {{ return cleanup_choice(v) }}

    rule ClauseB<<tokens>>:
                  {{ v = [] }}
                  ( ClauseC<<tokens>> {{ v.append(ClauseC) }} )*
                  {{ return cleanup_sequence(v) }}

    rule ClauseC<<tokens>>:
                  ClauseD<<tokens>>
                  ( PLUS {{ return Plus(ClauseD) }}
                  | STAR {{ return Star(ClauseD) }}
                  |      {{ return ClauseD }} )

    rule ClauseD<<tokens>>:
                  STR {{ t = (STR, eval(STR,{},{})) }}
                      {{ if t not in tokens: tokens.insert( 0, t ) }} 
                      {{ return Terminal(STR) }}
                | ID OptParam {{ return resolve_name(tokens, ID, OptParam) }}
                | LP ClauseA<<tokens>> RP {{ return ClauseA }}
                | LB ClauseA<<tokens>> RB {{ return Option(ClauseA) }}
                | STMT {{ return Eval(STMT[2:-2]) }}

    rule OptParam: [ ATTR {{ return ATTR[2:-2] }} ] {{ return '' }}
    rule Str:   STR {{ return eval(STR,{},{}) }}
%%

# This replaces the default main routine

yapps_options = [
    ('context-insensitive-scanner', 'context-insensitive-scanner',
     'Scan all tokens (see docs)')
    ]

def generate(inputfilename, outputfilename='', dump=0, **flags):
    """Generate a grammar, given an input filename (X.g)
    and an output filename (defaulting to X.py)."""

    if not outputfilename:
	if inputfilename[-2:]=='.g': outputfilename = inputfilename[:-2]+'.py'
	else: raise "Invalid Filename", outputfilename
        
    print '    SCONFIG   ', join(outputfilename.split('/')[-5:], '/')
    
    DIVIDER = '\n%%\n' # This pattern separates the pre/post parsers
    preparser, postparser = None, None # Code before and after the parser desc

    # Read the entire file
    s = open(inputfilename,'r').read()

    # See if there's a separation between the pre-parser and parser
    f = find(s, DIVIDER)
    if f >= 0: preparser, s = s[:f]+'\n\n', s[f+len(DIVIDER):]

    # See if there's a separation between the parser and post-parser
    f = find(s, DIVIDER)
    if f >= 0: s, postparser = s[:f], '\n\n'+s[f+len(DIVIDER):]

    # Create the parser and scanner
    p = ParserDescription(ParserDescriptionScanner(s))
    if not p: return
    
    # Now parse the file
    t = wrap_error_reporter(p, 'Parser')
    if not t: return # Error
    if preparser is not None: t.preparser = preparser
    if postparser is not None: t.postparser = postparser

    # Check the options
    for f in t.options.keys():
        for opt,_,_ in yapps_options:
            if f == opt: break
        else:
            print 'Warning: unrecognized option', f
    # Add command line options to the set
    for f in flags.keys(): t.options[f] = flags[f]
            
    # Generate the output
    if dump:
        t.dump_information()
    else:
        t.output = open(outputfilename, 'w')
        t.generate_output()

if __name__=='__main__':
    import sys, getopt
    optlist, args = getopt.getopt(sys.argv[1:], 'f:', ['dump'])
    if not args or len(args) > 2:
        print 'Usage:'
        print '  python', sys.argv[0], '[flags] input.g [output.py]'
        print 'Flags:'
        print ('  --dump' + ' '*40)[:35] + 'Dump out grammar information'
        for flag, _, doc in yapps_options:
            print ('  -f' + flag + ' '*40)[:35] + doc
    else:
        # Read in the options and create a list of flags
	flags = {}
	for opt in optlist:
	    for flag, name, _ in yapps_options:
		if opt == ('-f', flag):
		    flags[name] = 1
		    break
	    else:
                if opt == ('--dump', ''):
                    flags['dump'] = 1
		else:
                    print 'Warning - unrecognized option:  ', opt[0], opt[1]

        apply(generate, tuple(args), flags)
