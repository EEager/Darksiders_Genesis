#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	explicit CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_GamePlay() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_LightDesc();
	HRESULT Ready_Layer_Enviroment();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster();
	HRESULT Ready_Layer_Object();

private:
	 queue<function<bool(_float deltaTime)>> m_queueEventCallBack; // �̺�Ʈ �ݹ��Լ��� ��� �ִ� DB

private:
	// �ش� �̺�Ʈ�� �Ϸ�Ǿ��ٸ� true�� �����ϵ��� ����
	bool OnEvent1(_float fTimeDelta); // �ٸ���Ÿ ù ��� ���. ����Ʈ �߰�����.
	bool OnEvent2(_float fTimeDelta); // ������ ��. ��ġ �����ִ� ���.
	bool OnEvent3(_float fTimeDelta); // �� ���տ���, ���� ���ι� ������ ���. 
	bool OnEvent4(_float fTimeDelta); // ���� ��.


public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END