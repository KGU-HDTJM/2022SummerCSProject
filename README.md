# 2022SummerCSProject
KGU 2022 Summer CS C-With Project
### 본 프로젝트는 KGU C-With(2022 Summer) 프로젝트가 마무리 될 때까지 비공개 상태
> 프로젝트 이름은 파스칼 표기법을 따른다.
> 기타 문서나 리소스의 폴더는 카멜 표기법을 따른다.

# ⚠️Alert⚠️
## 프로그래밍 할때 항상 master branch에 있는 내부 문서들을 볼 수 있게 하고 특히 코딩표준 문서는 익숙해지기 전까지 코딩표준을 보면서 코딩표준에 맞춰서 코드를 작성한다.
  > 나중에 습관이니 뭐니 변명하면.. 그 코드 사용 못하게 할꺼임
## OpenGL하는 재민재 브라더스 master branch에 resource 폴더 안에 일자 테트리스 블럭이랑 obj파일에서 버텍스 읽어오는 dll 만들어 뒀으니까 링크는 properties에 새로 추가된 프로퍼티 시트 연결하면됨, 그래픽 강의 예제 따라가고 있는거 같던데 일단 모델링 된거 ReadVertexFromObjFile프로젝트에 있는 함수 참고해서 gl모델보다 resource에 있는 모델로 렌더링 해주고, 기본적인 변환 정도만 하고 컬러랑 텍스쳐까지 나가줘

## 코딩 표준 준수
### 현다재민 코딩 표준: https://github.com/KGU-HDJM/2022SummerCSProject/blob/master/docs/CodingStyleConvention.md
예제 코드를 가져다 쓰는 것이여도 분석하는 과정에서 코딩 표준에 맞게 수정 (변수, 함수, 구조체, 변수 등의 이름을 바꿔야 할때 **Ctrl + f** 로 변수를 선택하고 replace)

## 비주얼 스튜디오 2022 사용
## 기본적인 프로세스
### 새로운 프로젝트 생성
  > visual studio 새프로젝트 생성(Create a new project) -> 빈 프로젝트(Empty Project) 혹은 Windows Desktop Wizard -> Location은 항상 local에 있는 git폴더와 같은 위치로, 솔루션과 프로젝트를 같은 위치에 저장에 체크 -> 속성 관리자(propery manager)에서 properties 폴더에 있는 속성 시트 추가(기본적으로 HDJM2022SummerCS_x64.props는  추가해야함) -> 코드 작성
  
# Roles
## H(현석)
git repo, 개발 프로세스 관리
라이브러리 개발
## D(다현)
모델링, file io 관련 기능
## J(재민)
git repo 관리
## JM(재민재 브라더스)
OpenGL 그래픽스 렌더링

# 구현사항 (가능한 최신을 유지)
스택 dll라이브러리
