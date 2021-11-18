//
//	Author: 
//			burluckij@gmail.com
//			Burlutsky Stanislav
//

#pragma once

#include <boost/noncopyable.hpp>
#include "../../../../../../thrift/cpp/dgiBankingTypes_constants.h"
#include "../../../../../../thrift/cpp/dgiCommonTypes_types.h"
#include "../../../helpers/containers/ths_map.h"


namespace service
{
	namespace thrift_impl
	{
		//
		// �������� ������� � ����������� � ��������� ���������� ����������� ��������.
		//
		typedef ThsMap<dgi::AsyncId, dgi::DgiStatus::type> AsyncOperationTable;

		// ��������� ���������� � ������� ���������� ����������� ��������.
		class AsyncRequestsKeeper : private boost::noncopyable
		{
		public:

			AsyncRequestsKeeper();

			//
			// ���������� ������ � ��� ������, ���� ����� ������ � ��������� ������� ���������� (�����������).
			//
			bool isPresent(const dgi::AsyncId _requestId) const;

			//
			// ������� ������ �� ���������� ����� ����������� ���������.
			//
			dgi::AsyncId createNew(dgi::DgiStatus::type _requestState = dgi::DgiStatus::InProcess);

			//
			// ������� ��� ����������� �������.
			//
			void removeFinished();

			// Returns true if request is finished and have status unlike 'dgi::DgiStatus::InProcess'.
			//
			bool isFinished(dgi::AsyncId _requestId);

			//
			// ���������� ������ ���������� ����������� ��������. ���� ������ �������� �� ����������, ����� ����� false.
			//
			bool getState(const dgi::AsyncId _requestId, dgi::DgiStatus::type& _outRequestState) /* const */;

			//
			// ��������� ������ ���������� ����������� ���������.
			//
			void setState(const dgi::AsyncId _requestId, const dgi::DgiStatus::type _newRequestState);

			//
			// ������� ������� ��������, �� �� �������� ������� ��������, ��� ��������� ��������� (������������) ������ ������� � ���������
			// �����-������������ �������, � ����������� ����������������.
			//
			void clear();

		private:
			AsyncOperationTable m_operationStates;
			dgi::AsyncId m_counter;
			std::mutex m_counterLock;
		};

	}
}
