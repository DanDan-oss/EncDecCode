//#include "linuxshm.h"
//#include <sys/shm.h>
//#include <sys/ipc.h>
//#include <iostream>
//#include <string.h>
//
//
//
//shmBase::shmBase(int key)
//{
//	this->shm_init(key);
//	return;
//}
//shmBase::shmBase(int key, int size)
//{
//	this->shm_init(key, size);
//	return;
//}
//shmBase::shmBase(std::string pathname)
//{
//	this->shm_init(ftok(pathname.data(), 'x'));
//	return;
//}
//shmBase::shmBase(std::string pathname, int size)
//{
//	this->shm_init(ftok(pathname.data(), 'x'), size);
//	return;
//}
//shmBase::~shmBase()
//{
//	if (this->m_shmaddr)
//		shmdt(this->m_shmaddr);
//	return;
//}
//
//void shmBase::shm_init(int key)
//{
//	int shmid = 0;
//
//	shmid = shmget(key, 0, 0);
//	if (-1 == shmid)
//		return;
//	this->m_shmid = shmid;
//	return;
//}
//void shmBase::shm_init(int key, int size)
//{
//	int shmid = 0;
//	void* shm_addre = 0;
//	shmid = shmget(key, size, IPC_CREAT);
//	if (-1 == shmid)
//		return;
//	shm_addre = shmat(shmid, nullptr, 0);
//	if ((void*)-1 == shm_addre)
//		return;
//	memset(shm_addre, 0, size);
//	this->m_shmid = shmid;
//	this->m_shmaddr = shm_addre;
//	this->shm_unlock();
//	return;
//}
//
//void* shmBase::shm_locking()
//{
//	void* shm_addre = 0;
//
//	if (!this->m_shmid)
//		return 0;
//
//	if (this->m_shmaddr)
//		return this->m_shmaddr;
//
//	shm_addre = shmat(this->m_shmid, nullptr, 0);
//	if ((void*)-1 == shm_addre)
//		return nullptr;
//	this->m_shmaddr = shm_addre;
//	return shm_addre; 
//}
//
//void shmBase::shm_unlock()
//{
//	if (this->m_shmaddr)
//	{
//		shmdt(this->m_shmaddr);
//		this->m_shmaddr = 0;
//		return ;
//	}
//}
//
//void shmBase::shm_destroy()
//{
//	if (!this->m_shmid)
//		return;
//	shmctl(this->m_shmid, IPC_RMID, nullptr);
//}
//
////==========================================================
//sec_keyshm::sec_keyshm(int key, int maxnode)
//	: shmBase(key, maxnode+1 * sizeof(struct SHM__nodeinfo)), m_maxNode(maxnode)
//{
//
//}
//
//sec_keyshm::sec_keyshm(std::string pathname, int maxnode)
//	: shmBase(pathname, maxnode + 1 * sizeof(struct SHM__nodeinfo)), m_maxNode(maxnode)
//{
//	
//}
//
//int sec_keyshm::shm_write(struct SHM__nodeinfo* node_info)
//{
//	struct SHM__nodeinfo* pAddr = (struct SHM__nodeinfo*)this->shm_locking();
//
//	if (!pAddr)
//		return -1;
//	// 循环遍历,判断当前要写入的密钥是否已经存在需要覆盖写入
//	for (int i = 0; i < this->m_maxNode; ++i)
//		if (!strcmp(pAddr[i].clientID, node_info->clientID) && !strcmp(pAddr[i].serverID, node_info->serverID))
//		{	// 当前密钥已存在
//			memcpy(&pAddr[i], node_info, sizeof(struct SHM__nodeinfo));	//覆盖写入
//			this->shm_unlock();
//			return i;
//		}
//
//	// 当密钥不存在,循环遍历空节点
//	for (int i = 0; i < this->m_maxNode; ++i)
//		if (pAddr[i].status == 0)
//		{
//			memcpy(&pAddr[i], node_info, sizeof(struct SHM__nodeinfo));		// 在空节点/失效节点上插入新节点
//			this->shm_unlock();
//			return i;
//		}
//	this->shm_unlock();
//	return -1;
//}
//
//struct SHM__nodeinfo sec_keyshm::shm_read(std::string& clientID, std::string& serverID)
//{
//	struct SHM__nodeinfo shm__node;
//	struct SHM__nodeinfo* pAddr = (struct SHM__nodeinfo*)this->shm_locking();
//	
//	if (!pAddr)
//		return shm__node;
//	for (int i = 0; i < this->m_maxNode; ++i)
//		if (strcmp(pAddr[i].clientID, clientID.data())==0 && strcmp(pAddr[i].serverID, serverID.data())==0)
//		{
//			memcpy(&shm__node, &pAddr[i], sizeof(struct SHM__nodeinfo));
//			this->shm_unlock();
//			return shm__node;
//		}
//	this->shm_unlock();
//	return shm__node;
//}
//
//
