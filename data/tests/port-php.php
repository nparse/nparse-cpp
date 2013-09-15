<?php

$np = new nParse();

echo 'version:             ', $np->version(), "\n";
echo 'load:                ', $np->load('port.ng'), "\n";
echo 'parse:               ', $np->parse('hello world'), "\n";
echo 'status:              ', $np->status_str(), "\n";

echo 'message count:       ', $np->get_message_count(), "\n";
echo 'iteration count:     ', $np->get_iteration_count(), "\n";
echo 'trace count:         ', $np->get_trace_count(), "\n";
echo 'state pool usage:    ', $np->get_pool_usage(), " bytes\n";
echo 'state pool capacity: ', $np->get_pool_capacity(), " bytes\n";

while ($np->next()) {

	echo ':';

	while ($np->step()) {

		echo str_repeat(')', $np->shift());

		$node = $np->node();
		if ($node)
			echo " ($node";

		$text = $np->text();
		if ($text)
			echo " $text";
	}

	echo str_repeat(')', $np->shift()), "\n";
/*
	$cur = array();
	$stack = array();
	while (true) {
		$step = $np->step();
		for ($shift = $np->shift(); $shift > 0; -- $shift) {
			$top = array_pop($stack);
			end($top);
			$top[key($top)] = $cur;
			$cur = $top;
		}
		if (! $step)
			break;
		$node = $np->node();
		if ($node) {
			$cur[$node] = null;
			array_push($stack, $cur);
			$cur = array();
		}
		$text = $np->text();
		if ($text) {
			$cur[] = $text;
		}
	}
	echo json_encode($cur), "\n";
*/
}

for ($i = 0; $i < $np->get_message_count(); ++ $i) {
	echo "\t", $np->get_message($i), "\n";
	echo "\tin ", $np->get_location($i, $line, $offset), " : $line : $offset\n";
}

echo str_repeat('-', 20), "\n"; 
$np->rewind();
$np->next();
var_dump($np->get('z'));
