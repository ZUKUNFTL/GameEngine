#pragma once
#include "Hazel/Core.h"
#include "Layer.h"

#include <vector>

//��ϵͳ
namespace Hazel {

	class HAZEL_API LayerStack
	{

	public:
		LayerStack();
		~LayerStack();
		/*
			����ϣ���ڽ�����Ϸѭ����ʱ��ÿ�������õĲ㣨Layer�����ᰴ�ղ�ջ˳����¡�
			ͨ�����ָ���ѭ���������ڲ��Ͻ�����Ⱦ����Ȼ��Ϊ��ջ����˳��ģ������Ҫ��
			��ζ������԰Ѳ��������������棬����������˳�򡣲�ջ���ڹ���������ϵͳҲ�����á�
			����������Ѳ�������ջ�ĺ�벿�֡�
		*/
		//ע�����Ǹ���һ�� overlay����� overlay ��������˵�ĸ��㡣
		/*Ҳ����˵������һ�� layer������������ push ��ȥ��ͨ�����еĵ�����m_LayerInsert���룬
		m_LayerInsertҲ�������� layer �����һ��λ�ã���Ҫ���� overlay �����ǾͲ���Ҫ�ܵ�����
		m_LayerInsert ��λ�ã�����ֱ�� emplace_back ��ȥ����� LayerStack �е������������� �� 
		layer layer overlayer overlayer ���� m_LayerInsert ��Ӧ��ָ������ڶ��� layer ��λ�ã�
		�����һ���� overlayer �� layer��*/

		//������ͨ��
		void PushLayer(Layer* layer);
		//���븲��
		void PushOverlay(Layer* overlay);
		//�Ƴ���
		void PopLayer(Layer* layer);
		//�Ƴ�����
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;
		std::vector<Layer*>::iterator m_LayerInsert;
	};

}

