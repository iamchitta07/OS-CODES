#include "bits/stdc++.h"
#include "pthread.h"

using namespace std;

void *func(void *arg) {
    pthread_exit(arg);
}

int main() {
    int n;
    cout << "Enter the number of child threads: " << endl;
    cin >> n;

    pthread_t threads[n];
    int args[n];

    /////////////////////////////////////////////////////////////
    pthread_attr_t detachedThread;
    pthread_attr_init(&detachedThread);
    pthread_attr_setdetachstate(&detachedThread, PTHREAD_CREATE_DETACHED);

    for (int i = 0; i < n; i++) {        
        args[i] = i;

        if (i&1)
            pthread_create(&threads[i], &detachedThread, func, &args[i]);
        else
            pthread_create(&threads[i], NULL, func, &args[i]);
    }
    pthread_attr_destroy(&detachedThread);
    /////////////////////////////////////////////////////////////
    for (int i = 0; i < n; i++) {
        void *exitStatus;
        int resCode = pthread_join(threads[i], &exitStatus);
        if (resCode) {
            cout << "Unsuccessfull attempt to Join Thread" << endl;
        } else {
            cout << "The thread terminated passing the exit code :" << *((int *)exitStatus) << endl;
        }
    }
    return 0;
}