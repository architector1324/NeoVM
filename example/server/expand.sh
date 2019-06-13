#!/bin/bash

gcc -E -std=c11 -I ../../include/ main.c | grep -vE '^#' > main_e.c