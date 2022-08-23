# 2022SummerCSProject
KGU 2022 Summer CS C-With Project
### 본 프로젝트는 KGU C-With(2022 Summer) 프로젝트가 마무리 될 때까지 비공개 상태
> 프로젝트 이름은 파스칼 표기법을 따른다.
> 기타 문서나 리소스의 폴더는 카멜 표기법을 따른다.
## ⚠️프로젝트 내부의 모든 문서와 코드에 있는 주석에 잘못된 내용 혹은 모호하여 오해의 소지가 있는 부분은 문제를 인식한 사람이 다른 사람에게 물어서 책임지고 고친다.
# ⚠️Alert⚠️
## 프로그래밍 할때 항상 master branch에 있는 내부 문서들을 볼 수 있게 하고 특히 코딩표준 문서는 익숙해지기 전까지 코딩표준을 보면서 코딩표준에 맞춰서 코드를 작성한다.
  > 나중에 습관이니 뭐니 변명하면.. 그 코드 사용 못하게 할꺼임
### 깃허브 레포에  처음 푸시하는 사람 푸시하기전에 현다태재민 프로젝트 설정 방법하고 프로젝트 생성 후 깃에 푸시하는 과정 방법 아는 사람에게 배운다음에 깃 사용
## 코딩 표준 준수
### 현다재민 코딩 표준: https://github.com/KGU-HDJM/2022SummerCSProject/blob/master/docs/CodingStyleConvention.md
예제 코드를 가져다 쓰는 것이여도 분석하는 과정에서 코딩 표준에 맞게 수정 (변수, 함수, 구조체, 변수 등의 이름을 바꿔야 할때 **Ctrl + f** 로 변수를 선택하고 replace)

## 비주얼 스튜디오 2022 사용
## 기본적인 프로세스
  프로젝트와 관련된 모든 파일과 폴더는 git local repo에 두고 git local repo는 언제나 하나로 유지한다.
### 새로운 프로젝트 생성
  > visual studio 새 프로젝트 생성(Create a new project) -> 빈 프로젝트(Empty Project) 혹은 Windows Desktop Wizard -> Location은 항상 local에 있는 git폴더와 같은 위치로, 솔루션과 프로젝트를 같은 위치에 저장에 체크 -> 속성 관리자(propery manager)에서 properties 폴더에 있는 속성 시트 추가(기본적으로 HDJM2022SummerCS_x64.props는  추가해야함) -> 코드 작성
### dll 프로젝트 링크 하는법
  >  속성 관리자(propery manager)에서 properties에 <dll 프로젝트 이름>Link.props를 추가

# Roles
## H(현석)
git repo, 개발 프로세스 관리, 라이브러리 개발, OpenCL, GLSL 코드작성, x86_64 어셈블리
## D(다현)
모델링, file io 관련 기능
## T(태윤)
게임구현
## J(재민)
git repo 관리, opengl 그래픽스
## M(민재)
OpenGL 그래픽스 렌더링 파이프라인, draw call 최적화, 라이브러리 개발, GLSL 코드 작성, x86 어셈블리
