#!/usr/bin/env bash
HERE="$(dirname ${BASH_SOURCE[0]})"
SCORES_DIR="$HERE"
OUTPUT_DIR="out"

JSON_FILE=job.json
ADDED_SCORES_FILE="added_scores.txt"

rm  -f $JSON_FILE
rm -rf $OUTPUT_DIR
mkdir -p $OUTPUT_DIR

SCORES_TO_ADD_NUM=100

if [ -n "$1" ]; then
    SCORES_TO_ADD_NUM="$1"
fi

#echo "Scores to add: $SCORES_TO_ADD_NUM"

IFS=$'\n' read -d '' -r -a ADDED_SCORES < $ADDED_SCORES_FILE
#printf '%s\n' "${ADDED_SCORES[@]}"

counter=0
echo "[" >> $JSON_FILE

SCORES=$(find $(pwd) -name '*.mscz')
#echo "$SCORES"

for score in $SCORES; do
    if [ $counter -eq $SCORES_TO_ADD_NUM ]; then
        break
    fi

    if [ -f "$score" ]; then
        IN_FILE="$score"

        if [[ $(echo ${ADDED_SCORES[@]} | fgrep -w $IN_FILE) ]]; then
            #echo "Skip score: $IN_FILE"
            continue
        fi

        OUT_FILE="$(pwd)/$OUTPUT_DIR${score#$(pwd)}"
        OUT_FILE="${OUT_FILE%.*}.mp3"

        mkdir -p $(dirname "$OUT_FILE")

        echo "{ \"in\" : \"$IN_FILE\", \"out\" : \"$OUT_FILE\" }," >> $JSON_FILE;
        echo "$IN_FILE" >> $ADDED_SCORES_FILE;
    fi

    ((counter++))
done
echo "{}]" >> $JSON_FILE