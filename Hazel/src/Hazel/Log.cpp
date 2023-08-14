#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace Hazel {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		//�������ȷ�ķ�ʽ������ɫ��Ȼ����ʱ���%T��Ȼ����log������%n������������client��Ȼ����ʵ�ʵ���־��Ϣ%v%$
		spdlog::set_pattern("%^[%T] %n: %v%$");
		//����һ����־ʵ��
		s_CoreLogger = spdlog::stdout_color_mt("Hazel");
		//������־�ȼ�
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);

	}
}
