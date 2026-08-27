#!/bin/bash
set -e
mkdir -p .build 
mkdir -p .tmp
echo "Compiling cafeteria simulation binaries..."
gcc core/init_mq.c            -o .build/init_mq
gcc core/generator.c          -o .build/generator
gcc core/cashier.c            -o .build/cashier
gcc core/kitchen.c            -o .build/kitchen
gcc core/waiting_dispatcher.c -o .build/waiting_dispatcher
gcc core/table_manager.c      -o .build/table_manager
gcc core/dashboard.c          -o .build/dashboard
gcc core/control_panel.c      -o .build/control_panel

.build/init_mq
echo "Done! Binaries in .build/ and log file in ./.tmp"
