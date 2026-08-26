rm init_shm generator stall_worker decision dashboard

gcc fairqueue/init_shm.c -o  init_shm -lpthread
gcc fairqueue/generator.c -o  generator -lpthread
gcc fairqueue/stall_worker.c -o stall_worker -lpthread
gcc fairqueue/decision.c -o decision -lpthread
gcc fairqueue/dashboard.c -o dashboard -lpthread