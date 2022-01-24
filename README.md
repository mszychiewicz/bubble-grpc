# PORR
Projekt z przedmiotu PORR. Zrównoleglone sortowanie bąbelkowe z wykorzystaniem gRPC

### RUN

    export MY_INSTALL_DIR=$HOME/Packages/
    
    mkdir -p cmake/build    
    pushd cmake/build   
    cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..     
    make -j
    
    ./sort_server   
    ./sort_client