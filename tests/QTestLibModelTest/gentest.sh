#!/bin/sh
# Copyright 2015-2020 Pascal COMBES <pascom@orange.fr>
#
# This file is part of QTestLibPlugin.
#
# QTestLibPlugin is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# QTestLibPlugin is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with QTestLibPlugin. If not, see <http://www.gnu.org/licenses/>

## Usage ######################################################################
# gentest [Options] [--item [class [function [row]]] ...]
#
# Options:
#   -h|--help: Display this help
#   -q|--quiet: Run silently
#
# The test name will be used as is for the name of the folder containing
# test data and lower-cased for the name of the XML test and result files.
#
# There might be as may item as desired and they may optionally have a class
# a function of a row. Classes, functions and rows can be any string (it does
# not need to follow the rules of C++.
function usage()
{
    echo ""
    echo "gentest [Options] TestName[--item [class [function [row]]] ...]"
    echo ""
    echo "Options:"
    echo "   -h|--help: Display this help"
    echo "   -q|--quiet: Run silently"
    echo "   -v|--verbose: Run verbosely"
    echo ""
    echo "The test name will be used as is for the name of the folder containing"
    echo "test data and lower-cased for the name of the XML test and result files."
    echo ""
    echo "There might be as may item as desired and they may optionally have a class"
    echo "a function of a row. Classes, functions and rows can be any string (it does"
    echo "not need to follow the rules of C++."
    echo ""
    echo "Copyright 2015 Pascal COMBES <pascom@orange.fr>"
    echo ""
    exit 2
}

## Global variables ###########################################################
VERBOSE='normal' # Verbosity level ('debug', 'normal' or 'quiet')

## Echo functions #############################################################
# Display an error message and exit with code 1
#   $1: The error message
function error()
{
    echo -e "[\E[1;31mFAIL\E[0m] $1"
    exit 1
}

# Display a warning message
#   $1: The warning message
function warning()
{
    echo -e "[\E[1;33mWARN\E[0m] $1"
}

# Display a success message
#   $1: The success message
function success()
{
    echo -e "[\E[1;32m OK \E[0m] $1"
}

# Display an information message (in stderr) if verbosity is not 'quiet'
#   $1: The information message
function info()
{
    if [[ ! "$VERBOSE" == 'quiet' ]]; then
        echo -e "[INFO] $1" 1>&2
    fi
}

# Display a debug message (in stderr) if verbosity is 'debug'
#   $1: The debug message
function debug()
{
    if [[ "$VERBOSE" == 'debug' ]]; then
        echo -e "[DBUG] $1" 1>&2
    fi
}

## Argument parsing ###########################################################
# Options
if [[ $# -eq 0 || "$1" == '-h' || "$1" == '--help' ]]; then
    usage
fi
if [[ "$1" == '-v' || "$1" == '--verbose' ]]; then
    VERBOSE='debug'
    shift
fi
if [[ "$1" == '-q' || "$1" == '--quiet' ]]; then
    VERBOSE='quiet'
    shift
fi
# Folders and file names
TESTNAME="$1" # Name of the test (name of the test folder)
shift
TESTFILENAME=$(echo "${TESTNAME}test.xml" | tr '[:upper:]' '[:lower:]') # XML test file name
RESULTFILENAME=$(echo "${TESTNAME}.xml" | tr '[:upper:]' '[:lower:]')   # XML result file name
# Initialisation
CLASSES=()              # The classe of each item
FUNCTIONS=()            # The function of each item
ROWS=()                 # The row of each item
RESULTMESSAGECOUNT=()   # The number of the message for each item
CURRENTITEM=-1          # The number of item (the number of elements of the above array)
# Item argument parsing
while [[ $# -ne 0 ]]; do
    # New item
    if [[ "$1" == '--item' ]]; then
        CURRENTITEM=$((CURRENTITEM + 1))
        CLASSES[CURRENTITEM]=''
        FUNCTIONS[CURRENTITEM]=''
        ROWS[CURRENTITEM]=''
        RESULTMESSAGECOUNT[CURRENTITEM]=$((CURRENTITEM + 1))
        shift
        continue
    fi
    # Item was expected
    if [[ $CURRENTITEM -lt 0 ]]; then
        error "Item flag was expected"
    fi
    # The class name
    if [[ -z "${CLASSES[CURRENTITEM]}" ]]; then
        CLASSES[CURRENTITEM]="$1"
        shift
        continue
    fi
    # The function name
    if [[ -z "${FUNCTIONS[CURRENTITEM]}" ]]; then
        FUNCTIONS[CURRENTITEM]="$1"
        shift
        continue
    fi
    # The row name
    if [[ -z "${ROWS[CURRENTITEM]}" ]]; then
        ROWS[CURRENTITEM]="$1"
        shift
        continue
    fi
    # Too many data for the current item. Ignores.
    warning "Excedentary data \"$1\" ignored"
    shift
done
# Display results of argument parsing
info "Test name:        $TESTNAME"
info "Test file name:   $TESTFILENAME"
info "Result file name: $TESTNAME"
info "Classes:          ${CLASSES[*]}"
info "Functions:        ${FUNCTIONS[*]}"
info "Rows:             ${ROWS[*]}"
# Copy needed data to generate result file
ITEMNUMBER=$((CURRENTITEM + 1))
RESULTCLASSES=("${CLASSES[@]}")
RESULTFUNCTIONS=("${FUNCTIONS[@]}")
RESULTROWS=("${ROWS[@]}")
RESULTITEMNUMBER=$ITEMNUMBER
# Display the copies for the result file
info "Result classes:   ${RESULTCLASSES[*]}"
info "Result functions: ${RESULTFUNCTIONS[*]}"
info "Result rows:      ${RESULTROWS[*]}"
info "Result msg count: ${RESULTMESSAGECOUNT[*]}"
info "Result item count:$RESULTITEMNUMBER"

## Needed functions ##########################################################
# Does current item set need a root item ?
# No argument
# Return true (0) if current item set needs a root item.
function hasRoot()
{
    if [[ $ITEMNUMBER -eq 0 ]]; then
        return 0
    fi

    local class=''
    for c in "${CLASSES[@]}"; do
        if [[ -z "$c" ]]; then
            return 0
        fi
        if [[ -z "$CLASS" ]]; then
            local CLASS="$c"
            continue
        fi
        if [[ ! "$c" == "$CLASS" ]]; then
            return 0
        fi
    done

    return 1
}

# Remove item from result item set
#   $1: The index to remove
function removeResultItem()
{
    bs=0
    bl=$1
    es=$(($1 + 1))
    el=$((RESULTITEMNUMBER - $es + 1))
    RESULTCLASSES=("${RESULTCLASSES[@]:$bs:$bl}" "${RESULTCLASSES[@]:$es:$el}")
    RESULTFUNCTIONS=("${RESULTFUNCTIONS[@]:$bs:$bl}" "${RESULTFUNCTIONS[@]:$es:$el}")
    RESULTROWS=("${RESULTROWS[@]:$bs:$bl}" "${RESULTROWS[@]:$es:$el}")
    RESULTMESSAGECOUNT=(${RESULTMESSAGECOUNT[@]:$bs:$bl} ${RESULTMESSAGECOUNT[@]:$es:$el})
    RESULTITEMNUMBER=$((RESULTITEMNUMBER - 1))
}

# Number of items under root, class, function or row
#   $1:The current index (where the search will stop)
#   $2:The searched class name (optionnal)
#   $3:The searched function name (optionnal)
#   $4:The searched row name (optionnal)
# The number of already existing item under the given one
function itemsUnder()
{
    local c=0
    local i=0
    local childs=()

    while [[ $i < $1 ]]; do
        if [[ $# -eq 1 ]]; then
            local comp='root'
            local curComp='root'
            local curChild="${CLASSES[$i]}"
        elif [[ $# -eq 2 ]]; then
            local comp="$2"
            local curComp="${CLASSES[$i]}"
            local curChild="${FUNCTIONS[$i]}"
        elif [[ $# -eq 3 ]]; then
            local comp="$2::$3"
            local curComp="${CLASSES[$i]}::${FUNCTIONS[$i]}"
            local curChild="${ROWS[$i]}"
        elif [[ $# -eq 4 ]]; then
            local comp="$2::$3[$4]"
            local curComp="${CLASSES[$i]}::${FUNCTIONS[$i]}[${ROWS[$i]}]"
            local curChild=''
        else
            error "'itemsUnder' accepts only up to 4 arguments."
        fi

        debug "$comp =?= $curComp"
        if [[ "$comp" == "$curComp" ]]; then
            if [[ -z "$curChild" ]]; then
                local c=$((c + 1))
            else
                local child=0
                while [[ $child -lt ${#childs[@]} ]]; do
                    debug "Testing child $child \"${childs[$child]}\" -> \"$curComp($curChild)\""
                    if [[ "$curComp($curChild)" == "${childs[$child]}" ]]; then
                        debug "Equal"
                        break
                    fi
                    debug "Not equal"
                    local child=$((child + 1))
                done
                debug "$child"
                if [[ $child -eq ${#childs[@]} ]]; then
                    local childs[$child]="$curComp($curChild)"
                    local c=$((c + 1))
                    debug "Adding child ${childs[$child]} -> $curComp($curChild)"
                fi
            fi
        fi
        local i=$((i + 1))
        debug "Children: ${childs[*]}"
    done

    debug "$c"
    return $c
}

# Does class, function row already exists?
#   $1:The current index (where the search will stop)
#   $2:The name of the searched class
#   $3:The name of the searched function (optionnal)
#   $4:The name of the searched row (optionnal)
# True (0) if the item does not exist
function isNotUnder()
{
    debug "$# $@"
    local i=0

    while [[ $i < $1 ]]; do
        if [[ $# -eq 2 ]]; then
            local curChild="${CLASSES[$i]}"
            local curComp="$2"
        elif [[ $# -eq 3 ]]; then
            local curChild="${CLASSES[$i]}::${FUNCTIONS[$i]}"
            local curComp="$2::$3"
        elif [[ $# -eq 4 ]]; then
            local curChild="${CLASSES[$i]}::${FUNCTIONS[$i]}[${ROWS[$i]}]"
            local curComp="$2::$3[$4]"
        else
            error "'isNotUnder' accepts only up to 4 arguments."
        fi
        if [[ "$curChild" == "$curComp" ]]; then
            return 1
        fi
        local i=$((i + 1))
    done

    return 0
}

## Class, function and data row id management #################################
# Id arrays
CLASSIDS=('')       # Ids for classes
FUNCTIONIDS=('')    # Ids for functions
ROWIDS=('')         # Ids for rows

# Creates a new class id
# Warning: This function should not be invoked in a sub shell $() or ``
#   $1: The name of the class
# Return the id number for the class
function newClassId()
{
    local id=${#CLASSIDS[@]}
    CLASSIDS[$id]="$1"
    return $id
}

# The id for the given class
# Note: This function should be called in a sub shell $() or ``
#   $1: The name of the class
# True (0) if the class name was found
function classId()
{
    local i=1
    while [[ $i -lt ${#CLASSIDS[@]} ]]; do
        if [[ "${CLASSIDS[$i]}" == "$1" ]]; then
            echo "c$i"
            return 0
        fi
        local i=$((i + 1))
    done
    return 1
}

# Creates a new function id
# Warning: This function should not be invoked in a sub shell $() or ``
#   $1: The name of the class
#   $2: The name of the function
# Return the id number for the function
function newFunctionId()
{
    local id=${#FUNCTIONIDS[@]}
    FUNCTIONIDS[$id]="$1::$2"
    return $id
}

# The id for the given function
# Note: This function should be called in a sub shell $() or ``
#   $1: The name of the class
#   $2: The name of the function
# True (0) if the function name was found
function functionId()
{
    debug "ids:${FUNCTIONIDS[@]} nb:${#FUNCTIONIDS[@]} s:$1::$2"
    local i=1
    while [[ $i -lt ${#FUNCTIONIDS[@]} ]]; do
        if [[ "${FUNCTIONIDS[$i]}" == "$1::$2" ]]; then
            echo "f$i"
            return 0
        fi
        local i=$((i + 1))
    done
    return 1
}

# Creates a new row id
# Warning: This function should not be invoked in a sub shell $() or ``
#   $1: The name of the class
#   $2: The name of the function
#   $3: The title of the row
# Return the id number for the row
function newRowId()
{
    local id=${#ROWIDS[@]}
    ROWIDS[$id]="$1::$2[$3]"
    return $id
}

# The id for the given row
# Note: This function should be called in a sub shell $() or ``
#   $1: The name of the class
#   $2: The name of the function
#   $3: The title of the row
# True (0) if the row title was found
function rowId()
{
    debug "ids:${ROWIDS[@]} nb:${#ROWIDS[@]} s:$1::$2[$3]"
    local i=1
    while [[ $i -lt ${#ROWIDS[@]} ]]; do
        if [[ "${ROWIDS[$i]}" == "$1::$2[$3]" ]]; then
            echo "r$i"
            return 0
        fi
        local i=$((i + 1))
    done
    return 1
}

## Creation of folder #########################################################
# Creates the folder
if [[ -d "$TESTNAME" ]]; then
    error "Folder $TESTNAME already exists. Delete it beforehand"
fi
mkdir "$TESTNAME" || error "Could not create folder $TESTNAME"

## Creation of test result file ###############################################
# Initializes test result file
echo "<?xml version=\"1.0\" ?>" > "$TESTNAME/$RESULTFILENAME"
echo "<!DOCTYPE qtestliboutput SYSTEM \"../../../../qtestliboutput.dtd\">" >> "$TESTNAME/$RESULTFILENAME"
echo "<qtestliboutput>" >> "$TESTNAME/$RESULTFILENAME"

# Populates results
RESULTINDENT="    "

# Outputs a row in result file
#   $1: class name
#   $2: function name
#   $3: row title
function outputResultRow()
{
    newRowId "$1" "$2" "$3"
    local id=$?
    echo "$RESULTINDENT<row title=\"$3\" type=\"Unknown\" id=\"r$id\">" >> "$TESTNAME/$RESULTFILENAME"
    RESULTINDENT="$RESULTINDENT    "

    local i=0
    while [[ $i -lt $RESULTITEMNUMBER ]]; do
        debug "$i: $1 \"${RESULTCLASSES[$i]}\" \"${RESULTFUNCTIONS[$i]}\" \"${RESULTROWS[$i]}\""
        if [[ "${RESULTCLASSES[$i]}" == "$1" && "${RESULTFUNCTIONS[$i]}" == "$2" && "${RESULTROWS[$i]}" == "$3" ]]; then
            echo "$RESULTINDENT<message type=\"Unknown\" id=\"m${RESULTMESSAGECOUNT[$i]}\"><text strict=\"true\">Message ${RESULTMESSAGECOUNT[$i]}</text></message>" >> "$TESTNAME/$RESULTFILENAME"
            removeResultItem $i
        else
            local i=$((i + 1))
        fi
    done

    local l=$((${#RESULTINDENT} - 4))
    RESULTINDENT=${RESULTINDENT:0:$l}
    echo "$RESULTINDENT</row>" >> "$TESTNAME/$RESULTFILENAME"
    return 0
}

# Outputs a class in result file
#   $1: function name
#   $2: function name
function outputResultFunction()
{
    newFunctionId "$1" "$2"
    local id=$?
    echo "$RESULTINDENT<function name=\"$2\" type=\"Unknown\" id=\"f$id\">" >> "$TESTNAME/$RESULTFILENAME"
    RESULTINDENT="$RESULTINDENT    "

    local i=0
    while [[ $i -lt $RESULTITEMNUMBER ]]; do
        debug "$i: $1 \"${RESULTCLASSES[$i]}\" \"${RESULTFUNCTIONS[$i]}\" \"${RESULTROWS[$i]}\""
        if [[ "${RESULTCLASSES[$i]}" == "$1" && "${RESULTFUNCTIONS[$i]}" == "$2" ]]; then
            if [[ -z "${RESULTROWS[$i]}" ]]; then
                echo "$RESULTINDENT<message type=\"Unknown\" id=\"m${RESULTMESSAGECOUNT[$i]}\"><text strict=\"true\">Message ${RESULTMESSAGECOUNT[$i]}</text></message>" >> "$TESTNAME/$RESULTFILENAME"
                removeResultItem $i
            else
                outputResultRow "$1" "$2" "${RESULTROWS[$i]}"
            fi
        else
            local i=$((i + 1))
        fi
    done

    local l=$((${#RESULTINDENT} - 4))
    RESULTINDENT=${RESULTINDENT:0:$l}
    echo "$RESULTINDENT</function>" >> "$TESTNAME/$RESULTFILENAME"
    return 0
}

# Outputs a class in result file
#   $1: class name
function outputResultClass()
{
    newClassId "$1"
    local id=$?
    echo "$RESULTINDENT<class name=\"$1\" type=\"Unknown\" id=\"c$id\">" >> "$TESTNAME/$RESULTFILENAME"
    RESULTINDENT="$RESULTINDENT    "

    local i=0
    while [[ $i -lt $RESULTITEMNUMBER ]]; do
        debug "$i: $1 \"${RESULTCLASSES[$i]}\" \"${RESULTFUNCTIONS[$i]}\""
        if [[ "${RESULTCLASSES[$i]}" == "$1" ]]; then
            if [[ -z "${RESULTFUNCTIONS[$i]}" ]]; then
                echo "$RESULTINDENT<message type=\"Unknown\" id=\"m${RESULTMESSAGECOUNT[$i]}\"><text strict=\"true\">Message ${RESULTMESSAGECOUNT[$i]}</text></message>" >> "$TESTNAME/$RESULTFILENAME"
                removeResultItem $i
            else
                outputResultFunction "$1" "${RESULTFUNCTIONS[$i]}"
            fi
        else
            local i=$((i + 1))
        fi
    done

    local l=$((${#RESULTINDENT} - 4))
    RESULTINDENT=${RESULTINDENT:0:$l}
    echo "$RESULTINDENT</class>" >> "$TESTNAME/$RESULTFILENAME"
    return 0
}

# Outputs opening root tag if necessary
if hasRoot; then
    echo "$RESULTINDENT<root type=\"Unknown\" id=\"root\">" >> "$TESTNAME/$RESULTFILENAME"
    RESULTINDENT="$RESULTINDENT    "
fi

# Outputs contents of root
while [[ $RESULTITEMNUMBER -gt 0 ]]; do
    debug "Classes: (${RESULTCLASSES[*]})"
    if [[ -z "${RESULTCLASSES[0]}" ]]; then
        echo "$RESULTINDENT<message type=\"Unknown\" id=\"m${RESULTMESSAGECOUNT[0]}\"><text strict=\"true\">Message ${RESULTMESSAGECOUNT[0]}</text></message>" >> "$TESTNAME/$RESULTFILENAME"
        removeResultItem 0
    else
        outputResultClass "${RESULTCLASSES[0]}"
    fi
done

# Outputs closing root tag if necessary
if hasRoot; then
    l=$((${#RESULTINDENT} - 4))
    RESULTINDENT=${RESULTINDENT:0:$l}
    echo "$RESULTINDENT</root>" >> "$TESTNAME/$RESULTFILENAME"

fi

# End test result file
echo "</qtestliboutput>" >> "$TESTNAME/$RESULTFILENAME"
success "Created result file \"$TESTNAME/$RESULTFILENAME\""

## Creation of test file ######################################################
# Initializes test file
echo "<?xml version=\"1.0\" ?>" > "$TESTNAME/$TESTFILENAME"
echo "<!DOCTYPE qtestlibmodeltest SYSTEM \"../../../../qtestlibmodeltest.dtd\">" >> "$TESTNAME/$TESTFILENAME"
echo "<qtestlibmodeltest>" >> "$TESTNAME/$TESTFILENAME"
INDENT="    "

# items tag opening
echo "$INDENT<items>" >> "$TESTNAME/$TESTFILENAME"
INDENT="$INDENT    "

# Outputs items
i=0
while [[ $i -lt $ITEMNUMBER ]]; do 
    echo -n "$INDENT<item" >> "$TESTNAME/$TESTFILENAME"
    if [[ -n "${CLASSES[$i]}" ]]; then
        echo -n " class=\"${CLASSES[$i]}\"" >> "$TESTNAME/$TESTFILENAME"
    fi
    if [[ -n "${FUNCTIONS[$i]}" ]]; then
        echo -n " function=\"${FUNCTIONS[$i]}\"" >> "$TESTNAME/$TESTFILENAME"
    fi
    if [[ -n "${ROWS[$i]}" ]]; then
        echo -n " row=\"${ROWS[$i]}\"" >> "$TESTNAME/$TESTFILENAME"
    fi
    i=$((i + 1))
    echo ">Message $i</item>" >> "$TESTNAME/$TESTFILENAME"
done

# items tag closing
l=$((${#INDENT} - 4))
INDENT=${INDENT:0:$l}
echo "$INDENT</items>" >> "$TESTNAME/$TESTFILENAME"

# signals tag opening
echo "$INDENT<signals>" >> "$TESTNAME/$TESTFILENAME"
INDENT="$INDENT    "
ROOT=''

# Add a signal
#   $1: signal name
#   $2: type of signal arguments:
#       M: Argument is a QModelIndex
#       I: Argument is an Integer
#   $*: other arguments are values of signal arguments (QModelIndexes are passed by id).
function addSignal()
{
    ARGS=$1
    shift
    
    echo "$INDENT<signal name=\"$1\">" >> "$TESTNAME/$TESTFILENAME"
    INDENT="$INDENT    "
    shift
    
    local i=0
    while [[ i -lt ${#ARGS} && $# -gt 0 ]]; do
        case ${ARGS:$i:1} in
        M)
            echo "$INDENT<arg type=\"QModelIndex\" value=\"$1\" />" >> "$TESTNAME/$TESTFILENAME"
            shift
        ;;
        I)
            echo "$INDENT<arg type=\"Integer\" value=\"$1\" />" >> "$TESTNAME/$TESTFILENAME"
            shift
        ;;
        *)
            warning "Unknown argument type '${ARGS[$i]}' ignored"
            ;;
        esac
        local i=$((i + 1))
    done
    
    local l=$((${#INDENT} - 4))
    INDENT=${INDENT:0:$l}
    echo "$INDENT</signal>" >> "$TESTNAME/$TESTFILENAME"
}

# Output signal list
i=0
while [[ $i -lt $ITEMNUMBER ]]; do
    if [[ -z "${CLASSES[$i]}" ]]; then
        if [[ -z "$ROOT" ||  "$ROOT" == 'root' ]]; then
            # Insert message item in the root
            ROOT='root'
            itemsUnder $i
            CHILD=$?
            addSignal 'MII' 'rowsAboutToBeInserted' 'root' "$CHILD" "$CHILD"
            addSignal 'MII' 'rowsInserted' 'root' "$CHILD" "$CHILD"
        else
            # Replace root class by root and insert message item
            ID=$(classId "${ROOT:6}")
            if [[ $? -ne 0 ]]; then
                error "Class id not found: ${ROOT:6}"
            fi
            itemsUnder $i "${ROOT:6}"
            CHILD=$?
            ROOT='root'
            PREV=$((CHILD - 1))
            addSignal 'MM' 'dataChanged' 'root' 'root'
            addSignal 'MII' 'rowsAboutToBeInserted' 'root' "$CHILD" "$CHILD"
            addSignal 'MII' 'rowsInserted' 'root' "$CHILD" "$CHILD"
            addSignal 'MIIMI' 'rowsAboutToBeMoved' 'root' '0' "$PREV" "$ID" '0'
            addSignal 'MIIMI' 'rowsMoved' 'root' '0' "$PREV" "$ID" '0'
            addSignal 'MII' 'rowsAboutToBeInserted' 'root' '1' '1'
            addSignal 'MII' 'rowsInserted' 'root' '1' '1'
        fi
        i=$((i + 1))
        continue
    fi
      
    if [[ -z "$ROOT" ]]; then
        # Add root class
        ROOT="class:${CLASSES[$i]}"
        addSignal 'MM' 'dataChanged' 'root' 'root'
        CLASSID='root'
    elif [[ "$ROOT" == 'root' ]]; then
        if isNotUnder $i "${CLASSES[$i]}"; then
            # Add class under root (for non existing classes)
            itemsUnder $i
            CHILD=$?
            addSignal 'MII' 'rowsAboutToBeInserted' 'root' "$CHILD" "$CHILD"
            addSignal 'MII' 'rowsInserted' 'root' "$CHILD" "$CHILD"
        fi
        CLASSID=$(classId "${CLASSES[$i]}")
    elif [[ ! "$ROOT" == "class:${CLASSES[$i]}" ]]; then
        # Replace root class by root and insert class item
        ID=$(classId "${ROOT:6}")
        if [[ $? -ne 0 ]]; then
            error "Class id not found: ${ROOT:6}"
        fi
        itemsUnder $i "${ROOT:6}"
        CHILD=$?
        ROOT='root'
        PREV=$((CHILD - 1))
        addSignal 'MM' 'dataChanged' 'root' 'root'
        addSignal 'MII' 'rowsAboutToBeInserted' 'root' "$CHILD" "$CHILD"
        addSignal 'MII' 'rowsInserted' 'root' "$CHILD" "$CHILD"
        addSignal 'MIIMI' 'rowsAboutToBeMoved' 'root' '0' "$PREV" "$ID" '0'
        addSignal 'MIIMI' 'rowsMoved' 'root' '0' "$PREV" "$ID" '0'
        addSignal 'MII' 'rowsAboutToBeInserted' 'root' '1' '1'
        addSignal 'MII' 'rowsInserted' 'root' '1' '1'
        CLASSID=$(classId "${CLASSES[$i]}")
    fi
    
    if [[ -z "${FUNCTIONS[$i]}" ]]; then
        # Insert message item under class
        itemsUnder $i "${CLASSES[$i]}"
        CHILD=$?
        addSignal 'MII' 'rowsAboutToBeInserted' "$CLASSID" "$CHILD" "$CHILD"
        addSignal 'MII' 'rowsInserted' "$CLASSID" "$CHILD" "$CHILD"
        i=$((i + 1))
        continue
    fi

    if isNotUnder $i "${CLASSES[$i]}" "${FUNCTIONS[$i]}"; then
        # Insert function item
        itemsUnder $i "${CLASSES[$i]}"
        CHILD=$?
        addSignal 'MII' 'rowsAboutToBeInserted' "$CLASSID" "$CHILD" "$CHILD"
        addSignal 'MII' 'rowsInserted' "$CLASSID" "$CHILD" "$CHILD"
    fi
    FUNCTIONID=$(functionId "${CLASSES[$i]}" "${FUNCTIONS[$i]}")

    if [[ -z "${ROWS[$i]}" ]]; then
        # Insert message item under function
        itemsUnder $i "${CLASSES[$i]}" "${FUNCTIONS[$i]}"
        CHILD=$?
        addSignal 'MII' 'rowsAboutToBeInserted' "$FUNCTIONID" "$CHILD" "$CHILD"
        addSignal 'MII' 'rowsInserted' "$FUNCTIONID" "$CHILD" "$CHILD"
        i=$((i + 1))
        continue
    fi

    if isNotUnder $i "${CLASSES[$i]}" "${FUNCTIONS[$i]}" "${ROWS[$i]}"; then
        # Insert row item
        itemsUnder $i "${CLASSES[$i]}" "${FUNCTIONS[$i]}"
        CHILD=$?
        addSignal 'MII' 'rowsAboutToBeInserted' "$FUNCTIONID" "$CHILD" "$CHILD"
        addSignal 'MII' 'rowsInserted' "$FUNCTIONID" "$CHILD" "$CHILD"
    fi
    ROWID=$(rowId "${CLASSES[$i]}" "${FUNCTIONS[$i]}" "${ROWS[$i]}")

    # Insert message item (under row)
    itemsUnder $i "${CLASSES[$i]}" "${FUNCTIONS[$i]}" "${ROWS[$i]}"
    CHILD=$?
    addSignal 'MII' 'rowsAboutToBeInserted' "$ROWID" "$CHILD" "$CHILD"
    addSignal 'MII' 'rowsInserted' "$ROWID" "$CHILD" "$CHILD"
    i=$((i + 1))
done

# signals tag closing
l=$((${#INDENT} - 4))
INDENT=${INDENT:0:$l}
echo "$INDENT</signals>" >> "$TESTNAME/$TESTFILENAME"

# End test file
echo "</qtestlibmodeltest>" >> "$TESTNAME/$TESTFILENAME"
success "Created test file \"$TESTNAME/$TESTFILENAME\""

## Append result file to test result list #####################################
# Create test result list if needed
if [[ ! -e 'results.pri' ]]; then
    echo -n 'DISTFILES +=' > 'results.pri'
fi
# Append line for new result file
echo ' \' >> 'results.pri'
echo -n "    \$\$PWD/$TESTNAME/$RESULTFILENAME" >> 'results.pri'

## Append test file to test list ##############################################
# Create test list if needed
if [[ ! -e 'tests.pri' ]]; then
    echo -n 'DISTFILES +=' > 'tests.pri'
fi
# Append line for new test file
echo ' \' >> 'tests.pri'
echo -n "    \$\$PWD/$TESTNAME/$TESTFILENAME" >> 'tests.pri'

## Append test to test data list ##############################################
# Append line for new test file
echo "QTest::newRow(\"$TESTNAME\") << \"$TESTNAME\";" >> 'tests.lst'
success "Created test \"$TESTNAME\""
