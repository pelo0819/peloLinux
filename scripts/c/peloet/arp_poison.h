

void Poison();

void StartPoison(int soc, struct in_addr *tar1, struct in_addr *tar2);

void StopPoison();

void *PoisonTest(void * arg);

void SetOptArpThread(pthread_attr_t _attr, pthread_t _thread_id);

int isPoisoning();

void TransferPacket();

int maccmp(u_int8_t *from_mac, u_int8_t *to_mac);
