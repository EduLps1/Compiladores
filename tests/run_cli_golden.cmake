if(NOT DEFINED PROGRAM OR NOT DEFINED CASE OR NOT DEFINED SOURCE_DIR)
    message(FATAL_ERROR "PROGRAM, CASE e SOURCE_DIR sao obrigatorios")
endif()

if(CASE STREQUAL "valid_ast")
    set(command_args ast "${SOURCE_DIR}/examples/syntax-valid.bd1")
    set(expected_exit 0)
elseif(CASE STREQUAL "invalid_syntax")
    set(command_args parse "${SOURCE_DIR}/examples/syntax-errors.bd1")
    set(expected_exit 1)
elseif(CASE STREQUAL "demo")
    set(command_args demo syntax)
    set(expected_exit 0)
else()
    message(FATAL_ERROR "caso desconhecido: ${CASE}")
endif()

execute_process(
    COMMAND "${PROGRAM}" ${command_args}
    RESULT_VARIABLE actual_exit
    OUTPUT_VARIABLE actual_stdout
    ERROR_VARIABLE actual_stderr
)

file(READ "${SOURCE_DIR}/tests/expected/${CASE}.stdout" expected_stdout)
file(READ "${SOURCE_DIR}/tests/expected/${CASE}.stderr" expected_stderr)

if(NOT actual_exit EQUAL expected_exit)
    message(FATAL_ERROR
        "codigo de saida: esperado=${expected_exit}, obtido=${actual_exit}")
endif()
if(NOT actual_stdout STREQUAL expected_stdout)
    message(FATAL_ERROR
        "stdout divergiu\n--- esperado ---\n${expected_stdout}"
        "--- obtido ---\n${actual_stdout}")
endif()
if(NOT actual_stderr STREQUAL expected_stderr)
    message(FATAL_ERROR
        "stderr divergiu\n--- esperado ---\n${expected_stderr}"
        "--- obtido ---\n${actual_stderr}")
endif()
