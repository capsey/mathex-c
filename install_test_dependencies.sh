#!/bin/bash

VERSION=1.4.0

# Download the archive file
curl -O https://www.use-strict.de/sput-unit-testing/sput-$VERSION.tar.gz

# Extract the sput.h file from the archive
tar -zxvf sput-$VERSION.tar.gz sput-$VERSION/sput.h

# Delete the archive file
rm sput-$VERSION.tar.gz

# Move the sput.h to correct directory
mv sput-$VERSION/sput.h tests/sput.h

# Delete extracted directory
rmdir sput-$VERSION
