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
	 queue<function<bool(_float deltaTime)>> m_queueEventCallBack; // 이벤트 콜백함수를 담고 있는 DB

private:
	// 해당 이벤트가 완료되었다면 true를 리턴하도록 하자
	bool OnEvent1(_float fTimeDelta); // 바리스타 첫 대면 장면. 퀘스트 추가까지.
	bool OnEvent2(_float fTimeDelta); // 모험의 서. 경치 보여주는 장면.
	bool OnEvent3(_float fTimeDelta); // 성 문앞에서, 몬스터 삼인방 나오는 장면. 
	bool OnEvent4(_float fTimeDelta); // 보스 씬.


public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END