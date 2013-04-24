#! /usr/bin/perl -p

# This is needed for a nasty preinst hack to work around a bug in sarge's
# version of dpkg. It substitutes the literal text of conffiles into preinst
# scripts so that they can be used when moving conffiles between packages.

BEGIN {
    %texts = ();
    while (@ARGV > 1) {
	my $name = $ARGV[0];
	shift;
	local *FILE;
	open FILE, '<', $ARGV[0];
	local $/ = undef;
	my $text = <FILE>;
	close FILE;
	# Quote for the shell.
	$text =~ s/'/'\\''/g;
	shift;
	$texts{$name} = $text;
    }
}

for my $name (keys %texts) {
    s/\@$name\@/'$texts{$name}'/g;
}
