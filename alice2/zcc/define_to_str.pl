#!/usr/bin/perl

print "struct token_name {\n";
print "\tint token;\n";
print "\tchar *name;\n";
print "};\n\n";

print "\nstatic struct token_name token_list[] = {\n";

while (<>) {
    if (/^#define[\t ]+([A-Za-z_][A-Za-z0-9_]*)[\t ]+([0-9]+)/) {
	print "\t{ $2, \"$1\" },\n";
    }
}

print "};\n";
print "\n";
print "static int token_list_len = sizeof(token_list) / sizeof(token_list[0]);\n";

