#!/bin/sh

# Options
OPT=""

# Get script pwd
pushd $(dirname $0) > /dev/null
SCRIPTPATH=$(pwd)
popd > /dev/null

# Test folder management
if [[ ! -d 'tests' ]]; then
    mkdir 'tests'
fi
cd 'tests'

# 0 message items
#"$SCRIPTPATH/gentest.sh" $OPT testNone

read -r -d '' TESTS << EOD
#0items
None
#1item
I
Ic
Icf
Icfr
#2items
II
IcI
IcfI
IcfrI
IIc
IcIc
Ic1Ic2
IcfIc
Ic1fIc2
IcfrIc
Ic1frIc2
IIcf
IcIcf
Ic1Ic2f
IcfIcf
Icf1Icf2
Ic1fIc2f
Ic1f1Ic2f2
IcfrIcf
Icf1rIcf2
Ic1frIc2f
Ic1f1rIc2f2
IIcfr
IcIcfr
Ic1Ic2fr
IcfIcfr
Icf1Icf2r
Ic1fIc2fr
Ic1f1Ic2f2r
IcfrIcfr
Icfr1Icfr2
Icf1rIcf2r
Icf1r1Icf2r2
Ic1frIc2fr
Ic1fr1Ic2fr2
Ic1f1rIc2f2r
Ic1f1r1Ic2f2r2
#3itemsI
III
IcII
IcfII
IcfrII
IIcI
IcIcI
Ic1Ic2I
IcfIcI
Ic1fIc2I
IcfrIcI
Ic1frIc2I
IIcfI
IcIcfI
Ic1Ic2fI
IcfIcfI
Icf1Icf2I
Ic1fIc2fI
Ic1f1Ic2f2I
IcfrIcfI
Icf1rIcf2I
Ic1frIc2fI
Ic1f1rIc2f2I
IIcfrI
IcIcfrI
Ic1Ic2frI
IcfIcfrI
Icf1Icf2rI
Ic1fIc2frI
Ic1f1Ic2f2rI
IcfrIcfrI
Icfr1Icfr2I
Icf1rIcf2rI
Icf1r1Icf2r2I
Ic1frIc2frI
Ic1fr1Ic2fr2I
Ic1f1rIc2f2rI
Ic1f1r1Ic2f2r2I
#3itemsIc
IIIc
IcIIc
Ic1IIc2
IcfIIc
Ic1fIIc2
IcfrIIc
Ic1frIIc2
IIcIc
IIc1Ic2
IcIcIc
Ic1Ic1Ic2
Ic1Ic2Ic1
Ic1Ic2Ic2
IcfIcIc
Ic1fIc1Ic2
Ic1fIc2Ic1
Ic1fIc2Ic2
IcfrIcIc
Ic1frIc1Ic2
Ic1frIc2Ic1
Ic1frIc2Ic2
IIcfIc
IIc1fIc2
IcIcfIc
Ic1Ic1fIc2
Ic1Ic2fIc1
Ic1Ic2fIc2
IcfIcfIc
Ic1fIc1fIc2
Icf1Icf2Ic
Ic1f1Ic1f2Ic2
Ic1fIc2fIc1
Ic1fIc2fIc2
Ic1f1Ic2f2Ic1
Ic1f1Ic2f2Ic2
IcfrIcfIc
Ic1frIc1fIc2
Icf1rIcf2Ic
Ic1f1rIc1f2Ic2
Ic1frIc2fIc1
Ic1frIc2fIc2
Ic1f1rIc2f2Ic1
Ic1f1rIc2f2Ic2
IIcfrIc
IIc1frIc2
IcIcfrIc
Ic1Ic1frIc2
Ic1Ic2frIc1
Ic1Ic2frIc2
IcfIcfrIc
Ic1fIc1frIc2
Icf1Icf2rIc1
Icf1Icf2rIc2
Ic1fIc2frIc1
Ic1fIc2frIc2
Ic1f1Ic2f2rIc1
Ic1f1Ic2f2rIc2
IcfrIcfrIc
Ic1frIc1frIc2
Icfr1Icfr2Ic
Ic1fr1Ic1fr2Ic2
Icf1rIcf2rIc1
Icf1rIcf2rIc2
Icf1r1Icf2r2Ic
Ic1f1r1Ic1f2r2Ic2
Ic1frIc2frIc1
Ic1frIc2frIc2
Ic1fr1Ic2fr2Ic1
Ic1fr1Ic2fr2Ic2
Ic1f1rIc2f2rIc1
Ic1f1rIc2f2rIc2
Ic1f1r1Ic2f2r2Ic1
Ic1f1r1Ic2f2r2Ic2
#3itemsIcf
IIIcf
IcIIcf
Ic1IIc2f
IcfIIcf
Icf1IIcf2
Ic1fIIc2f
Ic1f1IIc2f2
IcfrIIcf
Icf1rIIcf2
Ic1frIIc2f
Ic1f1rIIc2f2
IIcIcf
IIc1Ic2f
IcIcIcf
Ic1Ic1Ic2f
Ic1Ic2Ic1f
Ic1Ic2Ic2f
IcfIcIcf
Icf1IcIcf2
Ic1fIc1Ic2f
Ic1f1Ic1Ic2f2
Ic1fIc2Ic1f
Ic1f1Ic2Ic1f2
Ic1fIc2Ic2f
Ic1f1Ic2Ic2f2
IcfrIcIcf
Icf1rIcIcf2
Ic1frIc1Ic2f
Ic1f1rIc1Ic2f2
Ic1frIc2Ic1f
Ic1f1rIc2Ic1f2
Ic1frIc2Ic2f
Ic1f1rIc2Ic2f2
IIcfIcf
IIcf1Icf2
IIc1fIc2f
IIc1f1Ic2f2
IcIcfIcf
IcIcf1Icf2
Ic1Ic1fIc2f
Ic1Ic1f1Ic2f2
Ic1Ic2fIc1f
Ic1Ic2f1Ic1f2
Ic1Ic2fIc2f
Ic1Ic2f1Ic2f2
IcfIcfIcf
Icf1Icf1Icf2
Ic1fIc1fIc2f
Ic1f1Ic1f1Ic2f2
Icf1Icf2Icf1
Icf1Icf2Icf2
Ic1f1Ic1f2Ic2f1
Ic1f1Ic1f2Ic2f2
Ic1fIc2fIc1f
Ic1f1Ic2f1Ic1f2
Ic1fIc2fIc2f
Ic1f1Ic2f1Ic2f2
Ic1f1Ic2f2Ic1f1
Ic1f1Ic2f2Ic1f2
Ic1f1Ic2f2Ic2f1
Ic1f1Ic2f2Ic2f2
IcfrIcfIcf
Icf1rIcf1Icf2
Ic1frIc1fIc2f
Ic1f1rIc1f1Ic2f2
Icf1rIcf2Icf1
Icf1rIcf2Icf2
Ic1f1rIc1f2Ic2f1
Ic1f1rIc1f2Ic2f2
Ic1frIc2fIc1f
Ic1f1rIc2f1Ic1f2
Ic1frIc2fIc2f
Ic1f1rIc2f1Ic2f2
Ic1f1rIc2f2Ic1f1
Ic1f1rIc2f2Ic1f2
Ic1f1rIc2f2Ic2f1
Ic1f1rIc2f2Ic2f2
IIcfrIcf
IIcf1rIcf2
IIc1frIc2f
IIc1f1rIc2f2
IcIcfrIcf
IcIcf1rIcf2
Ic1Ic1frIc2f
Ic1Ic1f1rIc2f2
Ic1Ic2frIc1f
Ic1Ic2f1rIc1f2
Ic1Ic2frIc2f
Ic1Ic2f1rIc2f2
IcfIcfrIcf
Icf1Icf1rIcf2
Ic1fIc1frIc2f
Ic1f1Ic1f1rIc2f2
Icf1Icf2rIc1f1
Icf1Icf2rIc1f2
Icf1Icf2rIc2f1
Icf1Icf2rIc2f2
Ic1fIc2frIc1f
Ic1f1Ic2f1rIc1f2
Ic1fIc2frIc2f
Ic1f1Ic2f1rIc2f2
Ic1f1Ic2f2rIc1f1
Ic1f1Ic2f2rIc1f2
Ic1f1Ic2f2rIc2f1
Ic1f1Ic2f2rIc2f2
IcfrIcfrIcf
Icf1rIcf1rIcf2
Ic1frIc1frIc2f
Ic1f1rIc1f1rIc2f2
Icfr1Icfr2Icf
Icf1r1Icf1r2Icf2
Ic1fr1Ic1fr2Ic2f
Ic1f1r1Ic1f1r2Ic2f2
Icf1r1Icf1r2Ic2f2
Icf1rIcf2rIc1f1
Icf1rIcf2rIc1f2
Icf1rIcf2rIc2f1
Icf1rIcf2rIc2f2
Icf1r1Icf2r2Icf1
Icf1r1Icf2r2Icf2
Ic1f1r1Ic1f2r2Ic2f1
Ic1f1r1Ic1f2r2Ic2f2
Ic1frIc2frIc1f
Ic1f1rIc2f1rIc1f2
Ic1frIc2frIc2f
Ic1f1rIc2f1rIc2f2
Ic1fr1Ic2fr2Ic1f
Ic1f1r1Ic2f1r2Ic1f2
Ic1fr1Ic2fr2Ic2f
Ic1f1r1Ic2f1r2Ic2f2
Ic1f1rIc2f2rIc1f1
Ic1f1rIc2f2rIc1f2
Ic1f1rIc2f2rIc2f1
Ic1f1rIc2f2rIc2f2
Ic1f1r1Ic2f2r2Ic1f1
Ic1f1r1Ic2f2r2Ic1f2
Ic1f1r1Ic2f2r2Ic2f1
Ic1f1r1Ic2f2r2Ic2f2
#3itemsIcfr
IIIcfr
IcIIcfr
Ic1IIc2fr
IcfIIcfr
Icf1IIcf2r
Ic1fIIc2fr
Ic1f1IIc2f2r
IcfrIIcfr
Icfr1IIcfr2
Icf1rIIcf2r
Icf1r1IIcf2r2
Ic1frIIc2fr
Ic1fr1IIc2fr2
Ic1f1rIIc2f2r
Ic1f1r1IIc2f2r2
IIcIcfr
IIc1Ic2fr
IcIcIcfr
Ic1Ic1Ic2fr
Ic1Ic2Ic1fr
Ic1Ic2Ic2fr
IcfIcIcfr
Icf1IcIcf2r
Ic1fIc1Ic2fr
Ic1f1Ic1Ic2f2r
Ic1fIc2Ic1fr
Ic1f1Ic2Ic1f2r
Ic1fIc2Ic2fr
Ic1f1Ic2Ic2f2r
IcfrIcIcfr
Icfr1IcIcfr2
Icf1rIcIcf2r
Icf1r1IcIcf2r2
Ic1frIc1Ic2fr
Ic1fr1Ic1Ic2fr2
Ic1f1rIc1Ic2f2r
Ic1f1r1Ic1Ic2f2r2
Ic1frIc2Ic1fr
Ic1fr1Ic2Ic1fr2
Ic1f1rIc2Ic1f2r
Ic1f1r1Ic2Ic1f2r2
Ic1frIc2Ic2fr
Ic1fr1Ic2Ic2fr2
Ic1f1rIc2Ic2f2r
Ic1f1r1Ic2Ic2f2r2
IIcfIcfr
IIcf1Icf2r
IIc1fIc2fr
IIc1f1Ic2f2r
IcIcfIcfr
IcIcf1Icf2r
Ic1Ic1fIc2fr
Ic1Ic1f1Ic2f2r
Ic1Ic2fIc1fr
Ic1Ic2f1Ic1f2r
Ic1Ic2fIc2fr
Ic1Ic2f1Ic2f2r
IcfIcfIcfr
Icf1Icf1Icf2r
Ic1fIc1fIc2fr
Ic1f1Ic1f1Ic2f2r
Icf1Icf2Icf1r
Icf1Icf2Icf2r
Ic1f1Ic1f2Ic2f1r
Ic1f1Ic1f2Ic2f2r
Ic1fIc2fIc1fr
Ic1f1Ic2f1Ic1f2r
Ic1fIc2fIc2fr
Ic1f1Ic2f1Ic2f2r
Ic1f1Ic2f2Ic1f1r
Ic1f1Ic2f2Ic1f2r
Ic1f1Ic2f2Ic2f1r
Ic1f1Ic2f2Ic2f2r
IcfrIcfIcfr
Icfr1IcfIcfr2
Icf1rIcf1Icf2r
Icf1r1Icf1Icf2r2
Ic1frIc1fIc2fr
Ic1fr1Ic1fIc2fr2
Ic1f1rIc1f1Ic2f2r
Ic1f1r1Ic1f1Ic2f2r2
Icf1rIcf2Icf1r
Icf1r1Icf2Icf1r2
Icf1rIcf2Icf2r
Icf1r1Icf2Icf2r2
Ic1f1rIc1f2Ic2f1r
Ic1f1r1Ic1f2Ic2f1r2
Ic1f1rIc1f2Ic2f2r
Ic1f1r1Ic1f2Ic2f2r2
Ic1frIc2fIc1fr
Ic1fr1Ic2fIc1fr2
Ic1f1rIc2f1Ic1f2r
Ic1f1r1Ic2f1Ic1f2r2
Ic1frIc2fIc2fr
Ic1fr1Ic2fIc2fr2
Ic1f1rIc2f1Ic2f2r
Ic1f1r1Ic2f1Ic2f2r2
Ic1f1rIc2f2Ic1f1r
Ic1f1r1Ic2f2Ic1f1r2
Ic1f1rIc2f2Ic1f2r
Ic1f1r1Ic2f2Ic1f2r2
Ic1f1rIc2f2Ic2f1r
Ic1f1r1Ic2f2Ic2f1r2
Ic1f1rIc2f2Ic2f2r
Ic1f1r1Ic2f2Ic2f2r2
IIcfrIcfr
IIcfr1Icfr2
IIcf1rIcf2r
IIcf1r1Icf2r2
IIc1frIc2fr
IIc1fr1Ic2fr2
IIc1f1rIc2f2r
IIc1f1r1Ic2f2r2
IcIcfrIcfr
IcIcfr1Icfr2
IcIcf1rIcf2r
IcIcf1r1Icf2r2
Ic1Ic1frIc2fr
Ic1Ic1fr1Ic2fr2
Ic1Ic1f1rIc2f2r
Ic1Ic1f1r1Ic2f2r2
Ic1Ic2frIc1fr
Ic1Ic2fr1Ic1fr2
Ic1Ic2f1rIc1f2r
Ic1Ic2f1r1Ic1f2r2
Ic1Ic2frIc2fr
Ic1Ic2fr1Ic2fr2
Ic1Ic2f1rIc2f2r
Ic1Ic2f1r1Ic2f2r2
IcfIcfrIcfr
IcfIcfr1Icfr2
Icf1Icf1rIcf2r
Icf1Icf1r1Icf2r2
Ic1fIc1frIc2fr
Ic1fIc1fr1Ic2fr2
Ic1f1Ic1f1rIc2f2r
Ic1f1Ic1f1r1Ic2f2r2
Icf1Icf2rIc1f1r
Icf1Icf2r1Ic1f1r2
Icf1Icf2rIc1f2r
Icf1Icf2r1Ic1f2r2
Icf1Icf2rIc2f1r
Icf1Icf2r1Ic2f1r2
Icf1Icf2rIc2f2r
Icf1Icf2r1Ic2f2r2
Ic1fIc2frIc1fr
Ic1fIc2fr1Ic1fr2
Ic1f1Ic2f1rIc1f2r
Ic1f1Ic2f1r1Ic1f2r2
Ic1fIc2frIc2fr
Ic1fIc2fr1Ic2fr2
Ic1f1Ic2f1rIc2f2r
Ic1f1Ic2f1r1Ic2f2r2
Ic1f1Ic2f2rIc1f1r
Ic1f1Ic2f2r1Ic1f1r2
Ic1f1Ic2f2rIc1f2r
Ic1f1Ic2f2r1Ic1f2r2
Ic1f1Ic2f2rIc2f1r
Ic1f1Ic2f2r1Ic2f1r2
Ic1f1Ic2f2rIc2f2r
Ic1f1Ic2f2r1Ic2f2r2
IcfrIcfrIcfr
Icfr1Icfr1Icfr2
Icf1rIcf1rIcf2r
Icf1r1Icf1r1Icf2r2
Ic1frIc1frIc2fr
Ic1fr1Ic1fr1Ic2fr2
Ic1f1rIc1f1rIc2f2r
Ic1f1r1Ic1f1r1Ic2f2r2
Icfr1Icfr2Icfr1
Icfr1Icfr2Icfr2
Icf1r1Icf1r2Icf2r1
Icf1r1Icf1r2Icf2r2
Ic1fr1Ic1fr2Ic2fr1
Ic1fr1Ic1fr2Ic2fr2
Ic1f1r1Ic1f1r2Ic2f2r1
Ic1f1r1Ic1f1r2Ic2f2r2
Icf1r1Icf1r2Ic2f2r1
Icf1r1Icf1r2Ic2f2r2
Icf1rIcf2rIc1f1r
Icf1r1Icf2r1Ic1f1r2
Icf1rIcf2rIc1f2r
Icf1r1Icf2r1Ic1f2r2
Icf1rIcf2rIc2f1r
Icf1r1Icf2r1Ic2f1r2
Icf1rIcf2rIc2f2r
Icf1r1Icf2r1Ic2f2r2
Icf1r1Icf2r2Icf1r1
Icf1r1Icf2r2Icf1r2
Icf1r1Icf2r2Icf2r1
Icf1r1Icf2r2Icf2r2
Ic1f1r1Ic1f2r2Ic2f1r1
Ic1f1r1Ic1f2r2Ic2f1r2
Ic1f1r1Ic1f2r2Ic2f2r1
Ic1f1r1Ic1f2r2Ic2f2r2
Ic1frIc2frIc1fr
Ic1fr1Ic2fr1Ic1fr2
Ic1f1rIc2f1rIc1f2r
Ic1f1r1Ic2f1r1Ic1f2r2
Ic1frIc2frIc2fr
Ic1fr1Ic2fr1Ic2fr2
Ic1f1rIc2f1rIc2f2r
Ic1f1r1Ic2f1r1Ic2f2r2
Ic1fr1Ic2fr2Ic1fr1
Ic1fr1Ic2fr2Ic1fr2
Ic1f1r1Ic2f1r2Ic1f2r1
Ic1f1r1Ic2f1r2Ic1f2r2
Ic1fr1Ic2fr2Ic2fr1
Ic1fr1Ic2fr2Ic2fr2
Ic1f1r1Ic2f1r2Ic2f2r1
Ic1f1r1Ic2f1r2Ic2f2r2
Ic1f1rIc2f2rIc1f1r
Ic1f1r1Ic2f2r1Ic1f1r2
Ic1f1rIc2f2rIc1f2r
Ic1f1r1Ic2f2r1Ic1f2r2
Ic1f1rIc2f2rIc2f1r
Ic1f1r1Ic2f2r1Ic2f1r2
Ic1f1rIc2f2rIc2f2r
Ic1f1r1Ic2f2r1Ic2f2r2
Ic1f1r1Ic2f2r2Ic1f1r1
Ic1f1r1Ic2f2r2Ic1f1r2
Ic1f1r1Ic2f2r2Ic1f2r1
Ic1f1r1Ic2f2r2Ic1f2r2
Ic1f1r1Ic2f2r2Ic2f1r1
Ic1f1r1Ic2f2r2Ic2f1r2
Ic1f1r1Ic2f2r2Ic2f2r1
Ic1f1r1Ic2f2r2Ic2f2r2
EOD

t=1
for TEST in $TESTS; do
    i=0
    args=''

    echo "[INFO] Test name: \"$TEST\""
    if [[ ${TEST:0:1} == '#' ]]; then
        continue
    fi

    if [[ ! $TEST == 'None' ]]; then
        while [[ $i -lt ${#TEST} ]]; do
            case ${TEST:$i:1} in
            I)
                args="$args --item"
                ;;
            c)
                args="$args class"
                ;;
            f)
                args="$args fun"
                ;;
            r)
                args="$args row"
                ;;
            1)
                args="${args}1"
                ;;
            2)
                args="${args}2"
                ;;
            *)
                echo -e "[\E[1;31mFAIL\E[0m] Unexpected argument '${TEST:$i:1}'"
                cd ..
                exit 1
                ;;
            esac
            i=$(( i + 1 ))
        done
    fi
    echo "[INFO] Arguments: \"$args\""
    "$SCRIPTPATH/gentest.sh" $OPT "test$t$TEST" $args
    t=$((t + 1))
done

# Go back to initial folder
cd ..

# 2 message items
#./gentest.sh $OPT II --item --item
#./gentest.sh $OPT IcI --item class --item
#./gentest.sh $OPT IcfI --item class function --item
#./gentest.sh $OPT IcfrI --item class function row --item
#./gentest.sh $OPT IIc --item --item class
#./gentest.sh $OPT IcIc --item class --item class
#./gentest.sh $OPT Ic1Ic2 --item class1 --item class2
#./gentest.sh $OPT IcfIc --item class function --item class
#./gentest.sh $OPT Ic1fIc2 --item class1 function --item class2
#./gentest.sh $OPT IcfrIc --item class function row --item class
#./gentest.sh $OPT Ic1frIc2 --item class1 function row --item class
#./gentest.sh $OPT IIcf --item --item class function
#./gentest.sh $OPT IcIcf --item class --item class function
#./gentest.sh $OPT Ic1Ic2f --item class1 --item class2 function
#./gentest.sh $OPT IcfIcf --item class function --item class function
#./gentest.sh $OPT Icf1Icf2 --item class function1 --item class function2
#./gentest.sh $OPT Ic1fIc2f --item class1 function --item class2 function
#./gentest.sh $OPT Ic1f1Ic2f2 --item class1 function1 --item class2 function2
#./gentest.sh $OPT IcfrIcf --item class function row --item class function
#./gentest.sh $OPT Icf1rIcf2 --item class function1 row --item class function2
#./gentest.sh $OPT Ic1frIc2f --item class1 function row --item class function
#./gentest.sh $OPT Ic1f1rIc2f2 --item class1 function1 row --item class function2
#./gentest.sh $OPT IIcfr --item --item class function row
#./gentest.sh $OPT IcIcfr --item class --item class function row
#./gentest.sh $OPT Ic1Ic2fr --item class1 --item class2 function row
#./gentest.sh $OPT IcfIcfr --item class function --item class function row
#./gentest.sh $OPT Icf1Icf2r --item class function1 --item class function2 row
#./gentest.sh $OPT Ic1fIc2fr --item class1 function --item class2 function row
#./gentest.sh $OPT Ic1f1Ic2f2r --item class1 function1 --item class2 function2 row
#./gentest.sh $OPT IcfrIcfr --item class function row --item class function row
#./gentest.sh $OPT Icfr1Icfr2 --item class function row1 --item class function row2
#./gentest.sh $OPT Icf1rIcf2r --item class function1 row --item class function2 row
#./gentest.sh $OPT Icf1r1Icf2r2 --item class function1 row1 --item class function2 row2
#./gentest.sh $OPT Ic1frIc2fr --item class1 function row --item class function row
#./gentest.sh $OPT Ic1fr1Ic2fr2 --item class1 function row1 --item class function row2
#./gentest.sh $OPT Ic1f1rIc2f2r --item class1 function1 row --item class function2 row
#./gentest.sh $OPT Ic1f1r1Ic2f2r2 --item class1 function1 row1 --item class function2 row2

# 3 message items
#./gentest.sh $OPT III --item --item --item
#./gentest.sh $OPT IcII --item class --item --item
#./gentest.sh $OPT IcfII --item class function --item --item
#./gentest.sh $OPT IcfrII --item class function row --item --item
#./gentest.sh $OPT IIcI --item --item class --item
#./gentest.sh $OPT IcIcI --item class --item class --item
#./gentest.sh $OPT Ic1Ic2I --item class1 --item class2 --item
#./gentest.sh $OPT IcfIcI --item class function --item class --item
#./gentest.sh $OPT Ic1fIc2I --item class1 function --item class2 --item
#./gentest.sh $OPT IcfrIcI --item class function row --item class --item
#./gentest.sh $OPT Ic1frIc2I --item class1 function row --item class --item
#./gentest.sh $OPT IIcfI --item --item class function --item
#./gentest.sh $OPT IcIcfI --item class --item class function --item
#./gentest.sh $OPT Ic1Ic2fI --item class1 --item class2 function --item
#./gentest.sh $OPT IcfIcfI --item class function --item class function --item
#./gentest.sh $OPT Icf1Icf2I --item class function1 --item class function2 --item
#./gentest.sh $OPT Ic1fIc2fI --item class1 function --item class2 function --item
#./gentest.sh $OPT Ic1f1Ic2f2I --item class1 function1 --item class2 function2 --item
#./gentest.sh $OPT IcfrIcfI --item class function row --item class function --item
#./gentest.sh $OPT Icf1rIcf2I --item class function1 row --item class function2 --item
#./gentest.sh $OPT Ic1frIc2fI --item class1 function row --item class function --item
#./gentest.sh $OPT Ic1f1rIc2f2I --item class1 function1 row --item class function2 --item
#./gentest.sh $OPT IIcfrI --item --item class function row --item
#./gentest.sh $OPT IcIcfrI --item class --item class function row --item
#./gentest.sh $OPT Ic1Ic2frI --item class1 --item class2 function row --item
#./gentest.sh $OPT IcfIcfrI --item class function --item class function row --item
#./gentest.sh $OPT Icf1Icf2rI --item class function1 --item class function2 row --item
#./gentest.sh $OPT Ic1fIc2frI --item class1 function --item class2 function row --item
#./gentest.sh $OPT Ic1f1Ic2f2rI --item class1 function1 --item class2 function2 row --item
#./gentest.sh $OPT IcfrIcfrI --item class function row --item class function row --item
#./gentest.sh $OPT Icfr1Icfr2I --item class function row1 --item class function row2 --item
#./gentest.sh $OPT Icf1rIcf2rI --item class function1 row --item class function2 row --item
#./gentest.sh $OPT Icf1r1Icf2r2I --item class function1 row1 --item class function2 row2 --item
#./gentest.sh $OPT Ic1frIc2frI --item class1 function row --item class function row --item
#./gentest.sh $OPT Ic1fr1Ic2fr2I --item class1 function row1 --item class function row2 --item
#./gentest.sh $OPT Ic1f1rIc2f2rI --item class1 function1 row --item class function2 row --item
#./gentest.sh $OPT Ic1f1r1Ic2f2r2I --item class1 function1 row1 --item class function2 row2 --item


#./gentest.sh $OPT IIIc --item --item --item class
#./gentest.sh $OPT IcIIc --item class --item --item class
#./gentest.sh $OPT Ic1IIc2 --item class --item --item class2
#./gentest.sh $OPT IcfIIc --item class function --item --item class
#./gentest.sh $OPT Ic1fIIc2 --item class function --item --item class2
#./gentest.sh $OPT IcfrIIc --item class function row --item --item class
#./gentest.sh $OPT Ic1frIIc2 --item class function row --item --item class2
#./gentest.sh $OPT IIcIc --item --item class --item class
#./gentest.sh $OPT IIc1Ic2 --item --item class --item class2
#./gentest.sh $OPT IcIcIc --item class --item class --item class
#./gentest.sh $OPT Ic1Ic1Ic2 --item class --item class --item class2
#./gentest.sh $OPT Ic1Ic2Ic1 --item class1 --item class2 --item class1
#./gentest.sh $OPT Ic1Ic2Ic2 --item class1 --item class2 --item class2
#./gentest.sh $OPT IcfIcIc --item class function --item class --item class
#./gentest.sh $OPT Ic1fIc1Ic2 --item class function --item class --item class2
#./gentest.sh $OPT Ic1fIc2Ic1 --item class1 function --item class2 --item class1
#./gentest.sh $OPT Ic1fIc2Ic2 --item class1 function --item class2 --item class2
#./gentest.sh $OPT IcfrIcIc --item class function row --item class --item class
#./gentest.sh $OPT Ic1frIc1Ic2 --item class1 function row --item class1 --item class2
#./gentest.sh $OPT Ic1frIc2Ic1 --item class1 function row --item class2 --item class1
#./gentest.sh $OPT Ic1frIc2Ic2 --item class1 function row --item class2 --item class2

#./gentest.sh $OPT IIcfI --item --item class function --item
#./gentest.sh $OPT IcIcfI --item class --item class function --item
#./gentest.sh $OPT Ic1Ic2fI --item class1 --item class2 function --item
#./gentest.sh $OPT IcfIcfI --item class function --item class function --item
#./gentest.sh $OPT Icf1Icf2I --item class function1 --item class function2 --item
#./gentest.sh $OPT Ic1fIc2fI --item class1 function --item class2 function --item
#./gentest.sh $OPT Ic1f1Ic2f2I --item class1 function1 --item class2 function2 --item
#./gentest.sh $OPT IcfrIcfI --item class function row --item class function --item
#./gentest.sh $OPT Icf1rIcf2I --item class function1 row --item class function2 --item
#./gentest.sh $OPT Ic1frIc2fI --item class1 function row --item class function --item
#./gentest.sh $OPT Ic1f1rIc2f2I --item class1 function1 row --item class function2 --item
#./gentest.sh $OPT IIcfrI --item --item class function row --item
#./gentest.sh $OPT IcIcfrI --item class --item class function row --item
#./gentest.sh $OPT Ic1Ic2frI --item class1 --item class2 function row --item
#./gentest.sh $OPT IcfIcfrI --item class function --item class function row --item
#./gentest.sh $OPT Icf1Icf2rI --item class function1 --item class function2 row --item
#./gentest.sh $OPT Ic1fIc2frI --item class1 function --item class2 function row --item
#./gentest.sh $OPT Ic1f1Ic2f2rI --item class1 function1 --item class2 function2 row --item
#./gentest.sh $OPT IcfrIcfrI --item class function row --item class function row --item
#./gentest.sh $OPT Icfr1Icfr2I --item class function row1 --item class function row2 --item
#./gentest.sh $OPT Icf1rIcf2rI --item class function1 row --item class function2 row --item
#./gentest.sh $OPT Icf1r1Icf2r2I --item class function1 row1 --item class function2 row2 --item
#./gentest.sh $OPT Ic1frIc2frI --item class1 function row --item class function row --item
#./gentest.sh $OPT Ic1fr1Ic2fr2I --item class1 function row1 --item class function row2 --item
#./gentest.sh $OPT Ic1f1rIc2f2rI --item class1 function1 row --item class function2 row --item
#./gentest.sh $OPT Ic1f1r1Ic2f2r2I --item class1 function1 row1 --item class function2 row2 --item

