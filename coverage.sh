#!/usr/bin/env bash
#
# Requires gcc and lcov installed, and the CC environment variable set to the
# gcc binary.
#
# * macOS: `brew install gcc lcov`
# * Debian/Ubuntu: `sudo apt-get install -y build-essential lcov`

# Define bash set builtins to ensure safer execution of this script.
set -Eeuo pipefail

# Set up defaults for CC and GCOV_PATH
export CC="${CC:-gcc-13}"
declare -r GCOV="${GCOV:-gcov-13}"
declare -r LCOV="${LCOV:-lcov}"

declare -r SAVED_DIR=$(pwd)
declare -r PROJECT_DIR=$(echo $(cd $(dirname $0); pwd))
# Use a separate build directory to not mess up our normal development workspace
declare -r BUILD_DIR="build-for-coverage/" # must match "binaryDir" in CMakePresets.json
declare -r TEST_DIR="test/"
declare -r COVERAGE_DIR="$PROJECT_DIR/coverage-report"
declare -r BINARIES_TO_RUN=(
    "$TEST_DIR/unity_testsuite"
)
declare -r FORCE_OPEN_REPORT_IN_BROWSER="no" # yes or no

if ! command -v "$CC" &> /dev/null; then
    echo "ERROR: The gcc binary '$CC' (CC=$CC) could not be found"
    echo
    echo "HOW TO FIX"
    echo "=========="
    echo "1. Add gcc to your system if needed."
    echo
    echo "    * macOS: brew install gcc"
    echo "    * Debian/Ubuntu: sudo apt-get install -y build-essential"
    echo
    echo "2. Set the \`CC\` env variable accordingly, e.g. \`CC=gcc-13\`"
    echo "   and run this script again."
    exit 1
fi

# We cannot simply parse the value of the CC environment variable (e.g.,
# "Is CC == gcc?"), because XCode on macOS will install `gcc` and `cc` binaries
# that are actually clang binaries.
declare -r DETECT_GCC=$("$CC" --version | head -n 1 | grep -i 'gcc')
if [ -z "$DETECT_GCC" ]; then
    echo "ERROR: Incompatible compiler for generating coverage reports"
    echo "============================================================"
    echo
    echo "This script must use GCC, but the CC environment variable is"
    echo "set to a different compiler:"
    echo
    echo "    CC=$CC"
    echo
    echo "The effective compiler for CC=$CC on your system is:"
    echo
    "$CC" --version
    echo
    echo "HOW TO FIX"
    echo "=========="
    echo "1. Add GCC to your system if needed."
    echo
    echo "    * macOS: brew install gcc"
    echo "    * Debian/Ubuntu: sudo apt-get install -y build-essential"
    echo
    echo "2. Set the \`CC\` env variable accordingly, e.g. \`CC=gcc-13\`"
    echo "   and run this script again."
    exit 2
fi

if ! command -v "$GCOV" &> /dev/null; then
    echo "ERROR: The gcov binary '$GCOV' (GCOV=$GCOV) could not be found"
    echo
    echo "HOW TO FIX"
    echo "=========="
    echo "1. Add gcov to your system if needed."
    echo
    echo "    * macOS: brew install gcc"
    echo "    * Debian/Ubuntu: sudo apt-get install -y build-essential"
    echo
    echo "2. Set the \`GCOV\` env variable accordingly, e.g. \`GCOV=gcov-13\`"
    echo "   and run this script again."
    exit 3
fi

if ! command -v "$LCOV" &> /dev/null; then
    echo "ERROR: The lcov binary '$LCOV' (LCOV=$LCOV) could not be found"
    echo
    echo "HOW TO FIX"
    echo "=========="
    echo "1. Add lcov to your system if needed."
    echo
    echo "    * macOS: brew install lcov"
    echo "    * Debian/Ubuntu: sudo apt-get install -y lcov"
    echo
    echo "2. Set the \`LCOV\` env variable accordingly, e.g. \`LCOV=lcov\`"
    echo "   and run this script again."
    exit 4
fi

rm -rf "$BUILD_DIR"
cmake --preset coverage # configure
cmake --build --preset coverage # build
cd "$BUILD_DIR"
# Clean-up counters from any previous run
"$LCOV" --zerocounters --directory .
# Run the tests we want to generate coverage reports for
for BINARY in "${BINARIES_TO_RUN[@]}"; do
    "$BINARY"
done
# Create coverage report by taking into account only the files contained in src/
"$LCOV" --capture --directory . -o coverage.info --include "$PROJECT_DIR/src/*" --gcov-tool "$GCOV"
# Create HTML report in the out/ directory
genhtml coverage.info --output-directory "$COVERAGE_DIR"
# Print coverage report to the terminal
"$LCOV" --list coverage.info

declare -r COVERAGE_REPORT_INDEX=$(realpath "$COVERAGE_DIR/index.html")
echo
echo "Coverage report in HTML format is available at $COVERAGE_REPORT_INDEX"

if [ "$FORCE_OPEN_REPORT_IN_BROWSER" = "yes" ]; then
    # Open HTML in default application (typically, your browser)
    open "$COVERAGE_REPORT_INDEX"
else
    # https://docs.github.com/en/actions/learn-github-actions/variables#default-environment-variables
    declare -r EXECUTED_WITHIN_CI="${CI:-false}"
    if [ "$EXECUTED_WITHIN_CI" = "true" ]; then
        echo "Running inside GitHub Actions, skipping browsing of coverage report"
    elif [ $(uname -r | sed -n 's/.*\( *Microsoft *\).*/\1/ip') ]; then
        # Script is running in Windows WSL, where we typically don't have access
        # to a GUI with browser
        echo
        echo "If running in WSL on Windows, you can mount the WSL disk as network drive"
        echo "and then access the report in your browser with a URL such as:"
        echo
        echo "    file:///U:${COVERAGE_REPORT_INDEX}"
    else
        # Open HTML in default application (typically, your browser)
        open "$COVERAGE_REPORT_INDEX"
    fi
fi

cd "$SAVED_DIR"
