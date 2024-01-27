#pragma once
#include "cIGZUnknown.h"
#include "EASTLConfigSC4.h"
#include "EASTL\vector.h"

class cIGZString;
class cISC4DepartmentBudget;
class cISC4LineItem;
class cISCPropertyHolder;
class SC4Percentage;

class cISC4BudgetSimulator : public cIGZUnknown
{
	public:
		class BudgetGroupInfo
		{
		public:
			intptr_t groupID;
			intptr_t unknown2; // Possibly an IGZString
			intptr_t unknown3; // Possibly a std::string
			intptr_t unknown4;
			intptr_t unknown5;
			intptr_t unknown6;
			intptr_t unknown7;
			intptr_t unknown8;
			intptr_t unknown9;
			intptr_t unknown10;
		};

		class BudgetItem
		{
		public:
			uint32_t departmentID;
			uint32_t lineItem;
			intptr_t unknown1;
			intptr_t unknown2;
			intptr_t unknown3;
			intptr_t unknown4;
			intptr_t unknown5;
		};

		class LoanInfo
		{
		public:
			intptr_t unknown1;
			intptr_t unknown2;
			intptr_t unknown3;
			intptr_t unknown4;
			intptr_t unknown5;
			intptr_t unknown6;
			intptr_t unknown7;
			intptr_t unknown8;
		};

		virtual bool Init(void) = 0;
		virtual bool Shutdown(void) = 0;

		virtual bool SetTotalFunds(int64_t llFunds) = 0;
		virtual int64_t GetTotalFunds(void) = 0;
		virtual bool DepositFunds(int64_t llFunds) = 0;
		virtual bool WithdrawFunds(int64_t llFunds) = 0;

		virtual int64_t GetMinAllowableFunds(void) = 0;

		virtual int64_t GetYTDIncome(void) = 0;
		virtual int64_t GetEstIncome(void) = 0;

		virtual int64_t GetYTDExpenses(void) = 0;
		virtual int64_t GetEstExpenses(void) = 0;

		virtual uint32_t GetTotalMonthlyExpense(void) = 0;
		virtual int64_t GetTotalYearlyExpense(void) = 0;

		virtual int32_t GetTotalMonthlyIncome(void) = 0;
		virtual int64_t GetTotalYearlyIncome(void) = 0;

		virtual int64_t GetTaxIncome(void) = 0;
		virtual int64_t GetTaxIncome(int32_t nTaxType) = 0;

		virtual int64_t SetTotalMonthlyExpense(int64_t llExpense) = 0;
		virtual int64_t SetTotalYearlyExpense(int64_t llExpense) = 0;

		virtual int64_t SetTotalMonthlyIncome(int64_t llIncome) = 0;
		virtual int64_t SetTotalYearlyIncome(int64_t llIncome) = 0;

		virtual bool ShowBudgetWindow(void) = 0;

		virtual cISC4DepartmentBudget* CreateDepartmentBudget(uint32_t dwDepartmentID, uint32_t dwBudgetGroup) = 0;
		virtual bool RemoveDepartmentBudget(uint32_t dwDepartmentID) = 0;
		virtual cISC4DepartmentBudget* GetDepartmentBudget(uint32_t dwDepartmentID) = 0;
		virtual cISC4DepartmentBudget* GetDepartmentBudget(cIGZString const& szDepartmentName) = 0;

		virtual bool GetAllGroups(eastl::vector<BudgetGroupInfo>& sGroups) = 0;

		virtual bool SetGroupName(uint32_t dwGroupID, cIGZString& szName) = 0;
		virtual bool GetDepartmentBudgetsInGroup(uint32_t dwGroupID, eastl::vector<cISC4DepartmentBudget*>& sGroups) = 0;

		virtual void NeededFundingChanged(cISC4DepartmentBudget* pDepartmentBudget) = 0;
		virtual void FundingPercentageChanged(cISC4DepartmentBudget* pDepartmentBudget) = 0;

		virtual int64_t GetFunding(uint32_t dwUnknownID) = 0; // Seems to always return zero
		virtual int64_t GetFunding(cISC4DepartmentBudget* pDepartmentBudget) = 0; // Same here

		virtual float GetTaxRate(uint32_t dwTaxGroup) = 0;
		virtual bool SetTaxRate(uint32_t dwTaxGroup, float fRate) = 0;

		virtual int64_t GetWeightedAssessedTaxValue(uint32_t dwTaxGroup) = 0;

		virtual int64_t GetBondIncrement(void) = 0;
		virtual bool IssueBond(uint32_t dwBondAmount) = 0;

		virtual int64_t GetTotalBorrowed(void) = 0;
		virtual int64_t GetCurrentBorrowingLimit(void) = 0;
		virtual int64_t GetCurrentBondLimit(void) = 0;
		virtual int64_t GetCurrentMaxOutstandingBondsLimit(void) = 0;
		virtual int64_t GetTotalMonthlyBondPayments(void) = 0;

		virtual bool GetAllLoans(eastl::vector<LoanInfo>& sLoans) = 0;

		virtual int32_t GetLoanTimeInMonths(void) = 0;
		virtual int64_t GetMonthlyPaymentForLoan(int64_t llLoanAmount) = 0;
		virtual int64_t GetFullCostOfLoan(int64_t llLoanAmount) = 0;

		virtual bool GetBudgetItemInfo(cISCPropertyHolder* pProperty, eastl::vector<BudgetItem>& sBudgetInfo) = 0;
		virtual bool GetBudgetItemForPurpose(cISCPropertyHolder* pProperty, uint32_t dwPurpose, BudgetItem& sBudgetItem) = 0;
		virtual bool ChangeBudgetItemLine(cISCPropertyHolder* pProperty, uint32_t dwPurpose, uint32_t dwUnknown) = 0;
		virtual bool ChangeBudgetItemCost(cISCPropertyHolder* pProperty, uint32_t dwPurpose, int64_t llCost) = 0;
		virtual bool ChangeBudgetItemLocalFunding(cISCPropertyHolder* pProperty, uint32_t dwPurpose, SC4Percentage const& sFunding) = 0;
		virtual bool AddBudgetItemToBudget(cISCPropertyHolder* pProperty, uint32_t dwPurpose) = 0;
		virtual bool RemoveBudgetItemToBudget(cISCPropertyHolder* pProperty, uint32_t dwPurpose) = 0;

		virtual cISC4LineItem* GetLineItemFromBudgetItem(BudgetItem& pBudgetItem) = 0;
		virtual bool CopyBudgetItemProperties(cISCPropertyHolder* pOriginal, cISCPropertyHolder* pCopy) = 0;
};