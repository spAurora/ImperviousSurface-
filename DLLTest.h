// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� DLLTEST_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// DLLTEST_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef DLLTEST_EXPORTS
#define DLLTEST_API __declspec(dllexport)
#else
#define DLLTEST_API __declspec(dllimport)
#endif


EXTERN_C DLLTEST_API void ShowPic();
EXTERN_C DLLTEST_API void StartSegment();

EXTERN_C DLLTEST_API void ShowHTree(int level);

EXTERN_C DLLTEST_API void ConfirmLevel();

EXTERN_C DLLTEST_API void Classify(int f_changeFrom, int f_changeTo, int f_whatFeature, double f_lowerLimit, double f_upperLimit);