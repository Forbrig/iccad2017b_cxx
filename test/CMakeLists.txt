
find_package(Boost COMPONENTS system filesystem REQUIRED)
IF (Boost_FOUND)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
	ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

add_executable(TestParse test_parse.cxx)
target_link_libraries(
  	TestParse
	Model
	Parser
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)

add_executable(TestRTree test_rtree.cxx)
target_link_libraries(
	TestRTree
	Model
	Parser
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)

add_executable(TestRTreeIntegration test_rtree_integration.cxx)
target_link_libraries(
	TestRTreeIntegration
	Model
	Parser
	rapidcheck
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)

add_executable(TestMST test_mst.cxx)
target_link_libraries(
	TestMST
	Model
	Parser
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)

add_executable(TestBase test_base.cxx)
target_link_libraries(
	TestBase
	Model
	Parser
	rapidcheck
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)

add_executable(TestGlobalRouter test_router.cxx)
target_link_libraries(
	TestGlobalRouter
	Model
	Parser
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)

add_executable(TestTreap test_treap.cxx)
target_link_libraries(
	TestTreap
  	Model
	rapidcheck
)

add_executable(TestTreapRadius2 test_treap_radius2.cxx)
target_link_libraries(
	TestTreapRadius2
  	Model
	rapidcheck
)

add_executable(TestTreapCircle test_treap_circle.cxx)
target_link_libraries(
	TestTreapCircle
  	Model
	rapidcheck
)


add_executable(TestGeometry test_geometry.cxx)
target_link_libraries(
	TestGeometry
  	Model 
	rapidcheck
)