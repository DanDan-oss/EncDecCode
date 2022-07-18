//#ifndef LINUX_SHM_H
//#define LINUX_SHM_H	// Linux �����ڴ�
//
//#define  SHM__FLAGE_READ 0x1
//#define  SHM__FLAGE_CREATE 0x3
//
//typedef struct SHM__nodeinfo
//{
//	unsigned int status = 0;		// ��Կ����״̬
//	unsigned int seckeyID = 0;		// ��ԿID
//	char clientID[15] = { 0 };		// �ͻ���ID
//	char serverID[15] = { 0 };		// ������ID
//	char seckey[128] = { 0 };		// ��Կ
//} SHM__NODEINFO, *PSHM__NODEINFO;
//
//class shmBase
//{
//public:
//	shmBase() {};
//	shmBase(int key);
//	shmBase(int key, int size);
//	shmBase(std::string pathname);
//	shmBase(std::string pathname, int size);
//	~shmBase();
//
//	void shm_init(int key);
//	void shm_init(int key, int size);
//	void* shm_locking();		// ����,���������ڴ�
//	void shm_unlock();			// ����,���빲���ڴ�
//	void shm_destroy();
//private:
//	int m_shmid = 0;
//	void* m_shmaddr = nullptr;
//};
//
//class sec_keyshm : public shmBase
//{
//public:
//	sec_keyshm(int key, int maxnode);
//	sec_keyshm(std::string pathname, int maxnode);
//	int shm_write(struct SHM__nodeinfo* node_info);
//	struct SHM__nodeinfo  shm_read(std::string& clientID, std::string& serverID);
//private:
//	int m_maxNode;
//};
//
//
//
//#endif // LINUX_SHM_H
