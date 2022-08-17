#!/bin/bash

git show ':!BM1684EVB' ':!LF1704_BM1684_SE5V2.X' ':!LF1704_BM1684_SE5.X' ':!SA5AIUC10' ':!SC5PLUS' | \
        scripts/checkpatch.pl
