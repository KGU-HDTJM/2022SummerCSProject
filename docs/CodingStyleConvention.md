# Team 현다재민(HDJM)
# 코딩 표준 (Coding Style Convention)
## 기본원칙
1. 가독성을 최우선으로 삼는다. (대부분의 경우 코드는 그 자체가 문서의 역할을 해야 함)
2.	문제가 있을 경우 최대한 빨리 크래시가 나거나 assert에 걸리도록 코드를 작성한다. 어떻게든 프로그램을 돌게 만들다가 나중에 크래시가 나는 것보다 나쁜 상황은 없다.
3.	정말 합당한 이유가 있지 않는 한, 통합개발환경(IDE)의 자동 서식을 따른다. (비주얼 스튜디오의 "Ctrl + K + D" 기능)
4.	본 코딩표준을 따라 잘 짜여진 기존의 코드에서 배운다.

## 참조문서
1. C++ 코딩 표준 (https://docs.popekim.com/ko/coding-standards/cpp)
2. C 코딩 표준 (https://docs.popekim.com/ko/coding-standards/pocu-c)
3. DOOM Source code (https://github.com/id-Software/DOOM)
4. Windows via C/C++ 5th Edition

## 메인 코딩 표준
1. 구조체의 이름은 파스칼 표기법을 따르며 선언의 끝에 _s를 붙인다.
    ```C
    struct Vector2f_s
    {
        float X;
        float Y;
    };
    ```
2. 지역 변수 그리고 함수의 매개 변수의 이름은 카멜 표기법을 따른다.
    ```C
    int Function(const int parameter)
    {
        int ret;
        int number;
        int id;
    }
    ```
3. 함수의 이름은 동사로 시작하며 파스칼 표기법을 따른다.
    ```C 
    void DoSomething(void);
    ```
5. 상수 또는 #define 으로 정의된 상수의 이름은 모두 대문자로 하되 밑줄로 각 단어를 분리한다.
    ```C
    #define PI_HALF 1.5707963267948966192313216916398F
    const int INT_MAX = 0x7fffffff
    ```
6. 부울(boolean)형 변수는 앞에 b를 붙인다.
    ```C
    bool bState0;
    BOOL bState1;
    ```
7. 단순히 반복문에 사용되는 변수가 아닌 경우엔 i, e 같은 변수명 대신 index, employee 처럼 변수에 저장되는 데이터를 한 눈에 알아볼 수 있는 변수명을 사용한다.
8. typedef의 끝에 _t를 붙인다.
    ```C
    typedef unsigned int uint_t;
    typdef struct Vector3f_s
    {
        float X;
        float Y;
        float Z;
    } Vector3f_t;
    ```
9. #define으로 정의한 표현식은 소괄호로 감싼다.
    ```C
    #define Add(x, y) (x + y)
    #define SOME_BIT_FLAG (1 << 8)
    ```
10. 언제나 함수 원형으로 반환형을 선언한다. 컴파일러가 지멋대로 추측하는 일은 없도록 한다.
    ``` C
    // 잘못된 예
    Func(int x);
    // 옳은 예
    int Func(int x);
    ```
11. 값을 반환하는 함수의 이름은 무엇을 반환하는지 알 수 있게 짓는다.
12.	매개변수를 받지 않는 함수는 매개변수 목록에 반드시 void를 넣어준다.
    ```C
    // 잘못된 예
    void Fucn();
    // 옳은 예
    void Func(void);
    ```
13.	널 포인터를 반환하는 함수의 이름은 OrNULL로 끝나야 한다
14.	널 포인터를 허용하는 매개변수의 이름은 OrNULL로 끝나야 한다.
15.	switch 문 안에 언제나 default 케이스를 넣는다.
16.	일부러 case 문을 다음 case 문으로 넘어가게 할 때에는 /* intentional fallthrough */ 이란 주석을 넣는다.
    ```C
    switch (state)
    {
    case 0:
    break;
    case 1:
    /* intentional fallthrought */
    case 2:
        break;
    default:
        break;
    }
    ```
17.	모든 case 문은 break 키워드를 사용한다. 단, 일부러 case 문을 다음 case 문으로 넘어가게 하고 싶을 때는 예외로 한다.
18.	switch 문에서 default 케이스가 일어나지 말아야 한다면 assert(0)을 사용한다.
19.	외부 헤더파일을 인클루드 할 때는 #include<>를 사용한다. 자체 개발한 헤더파일은 #include "" 로 인클루드한다.
20.	외부 헤더파일을 먼저, 그리고 자체 개발한 헤더파일을 인클루드 한다. 이 때 가능하면 알파벳 순서를 따른다.
21.	최대한 const 키워드를 많이 사용한다. 이는 지역 변수나 함수 매개변수에도 마찬가지이다.
22.	const 값형을 반환하지 않는다. const 반환은 포인터에만 쓴다.
23.	불리언 비교에 숫자를 그대로 사용하지 않는다. 그 대신 0과 비교를 한다.
24.	널 포인터를 표현할 때는 숫자 0 대신 NULL을 #define해 놓은 것을 사용한다.
    ```C
    // 잘못된 예
    void* buf = malloc(BUFFER_SIZE);
        if (buf == 0) { /* error */ }
    // 옳은 예
    void* buf = malloc(BUFFER_SIZE);
        if (buf == NULL) { /* error */ }
    ```
25.	포인터가 out 매개변수로 사용될 경우 매개변수 이름 앞에 Out을 붙인다.
26.	위 out 매개변수는 널 포인터가 아니어야 한다. (이 때, if문이 아니라 assert를 쓸 것)
27.	모든 컴파일러 경고(warning)은 고친다.
28.	goto 레이블 명은 모두 대문자로 하되 밑줄로 각 단어를 분리한다.
    ``` C
    goto RELEASE_BUFFER;
    RELEASE_BUFFER:
    ```
29.	뒤에 추가적인 단어가 오지 않는 경우 줄임말은 모두 대문자로 표기한다.
30.	구조체의 멤버 변수명은 파스칼 표기법을 따르며, 구조체 안에서 함수는 사용하지 않는다.
31.	모든 헤더 파일 첫 번째 줄에 #pragma once를 기재한다.
32.	지역 변수를 선언할 때는 그 지역 변수를 사용하는 코드와 동일한 줄에 선언하는 것을 원칙으로 한다.
33.	double이 반드시 필요한 경우가 아닌 이상 부동 소수점 값에 F를 붙여준다.
    ```C
    #define PI 3.14F
    ```
34.	재귀 함수는 이름 뒤에 Recursive를 붙인다.
    ```C
    int GetFibonacciNumberRecursive(int n);
    ```
35.	독립된 기능은 각각 독립된 소스 파일에 있어야 한다. 단, 작은 기능 몇 개를 한 파일 안에 같이 넣어두는 것이 상식적일 경우 예외를 허용한다.
36.	표준 C assert 대신에 자신만의 Assert 버전을 구현한다. (Assert 구현 담당: 아마도 신현석)
37.	모든 메모리 할당은 자체제작한 함수를 사용한다.
38.	비트 플래그 열거형은 이름 뒤에 Flags를 붙인다.
39.	특정 크기(예를 들어, 데이터 멤버의 직렬화를 위한 크기)가 필요하지 않은 한 열거형에 크기 지정자를 추가하지 않는다.
40.	한 줄에 변수 하나만 선언한다.
41.	전역 변수의 선언은 헤더파일 안에서 extern 키워드를 사용하여 한다.
42.	변수는 가능한 최소한의 범위로 제한한다.
43.	한 함수에서만 사용되는 변수는 반드시 그 함수 안에서 지역변수나 정적변수로 선언되어야 한다.
44.	한 파일에 존재하는 여러 함수에서만 사용하는 변수가 있다면 그 변수는 그 파일 안에서 정적 변수로 선언되어야 한다.
45.	여러 소스 파일에서 사용하는 변수들만 전역 변수로 선언한다.
46.	초기화 목록에서 요소를 생략하여 배열을 0으로 초기화 할 때는 0 이후에 쉼표(,)를 넣어 의도가 명확히 보이도록 한다.
47.	동적으로 할당한 메모리 주소를 저장하고 있는 포인터 변수를 연산에 사용할 때는 그 포인터의 사본을 만들어 사용한다. 이는 잘못된 메모리 해제를 막기 위함이다.
48.	내부에서 동적으로 메모리를 할당하는 함수의 이름은 반드시 Malloc으로 끝난다.
49.	void 형 함수는 실패하지 않는다. 실패 할 수 있는 함수가 어쩔 수 없이 void형 이여야만 하는 함수는 에러를 알려줄 변수를 포인터로 받는다.
50.	에러를 나타나는 상수는 define으로 헤더파일에 정의하여 사용하며 ERROR_*기능이름* 으로 시작한다.
51.	열거형 상수는 언제나 열거형 이름으로 시작하여야 한다.
52. inline 함수는 헤더 파일에 #define으로 구현한다.
## 소스 코드 포맷팅
1. include 전처리문 블록과 코드 본문 사이에 반드시 빈 줄이 있어야 한다.
2. 탭(tab)은 비주얼 스튜디오 기본값을 사용하며, 비주얼 스튜디오를 사용하지 않을 시 띄어쓰기 4칸을 탭으로 사용한다.
3. 중괄호( { )를 열 때는 언제나 새로운 줄에 연다.
4. 중괄호 안( { } )에 코드가 한 줄만 있더라도 반드시 중괄호를 사용한다.
5. 포인터는 자료형에 붙인다.
    ```C
    int* pInteger;
    ```
## 프로젝트 설정 및 프로젝트 구조
1. Visual C++: 프로젝트 설정을 변경하려면 항상 속성 시트(property sheets)에서 변경 한다.
2. 프로젝트 설정에서 컴파일 경고를 비활성화 하지 않는다. 그 대신, 코드에서 #pragma 를 사용한다. ( #pragma warning (disable : error_or_warning_code) )
3. 프로젝트 내부에서 사용되는 실행 파일과 라이브러리 파일의 출력 위치를 통일한다.(property sheet 작성)
4. 프로젝트 설정은 master와 2인 이상이 동시에 작업하는 repository에 commit 및 push할 때는 무시한다.
    > vcxproj파일들 제외하고 commit
