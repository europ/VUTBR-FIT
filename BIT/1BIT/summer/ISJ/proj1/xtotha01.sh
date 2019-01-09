#!/usr/bin/env bash
if [ $# -eq 0 ]; then
	sed -e '/xkcd/ s/bu.*ls/\[gikuj\]\.\.n\|a\.\[alt\]\|\[pivo\]\.l\|i\.\.o\|\[jocy\]e\|sh\|di\|oo/' -e '/[[]/ {/[=]/ {/[!]/ ! s/xkcd/xtotha01/g}}' xkcd1313.ipynb > xtotha01.ipynb
else
	sed -e '/xkcd/ s/bu.*ls/\[gikuj\]\.\.n\|a\.\[alt\]\|\[pivo\]\.l\|i\.\.o\|\[jocy\]e\|sh\|di\|oo/' -e '/[[]/ {/[=]/ {/[!]/ ! s/xkcd/xtotha01/g}}' $1 > xtotha01.ipynb
fi