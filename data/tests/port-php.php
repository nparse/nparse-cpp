<?php

$np = new nParse();

echo 'version:               ', $np->version(), "\n";
echo 'load:                  ', $np->load('classic/port.ng'), "\n";
echo 'parse:                 ', $np->parse('hello world'), "\n";
echo 'status:                ', $np->status_str(), "\n";

echo 'message count:         ', $np->get_message_count(), "\n";
echo 'iteration count:       ', $np->get_iteration_count(), "\n";
echo 'trace count:           ', $np->get_trace_count(), "\n";
echo 'state pool usage:      ', $np->get_pool_usage(), " bytes\n";
echo 'state pool peak usage: ', $np->get_pool_peak_usage(), " bytes\n";
echo 'state pool capacity:   ', $np->get_pool_capacity(), " bytes\n";

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
}

for ($i = 0; $i < $np->get_message_count(); ++ $i) {
    echo "\t", $np->get_message($i), "\n";
    echo "\tin ", $np->get_location($i, $line, $offset), " : $line : $offset\n";
}

echo str_repeat('-', 20), "\n";
$np->rewind();
$np->next();
var_dump($np->get('z'));
