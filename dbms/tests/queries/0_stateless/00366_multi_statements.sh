#!/usr/bin/env bash

CURDIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
. $CURDIR/../shell_config.sh

$CLICKHOUSE_CLIENT --query="SELECT 1"
$CLICKHOUSE_CLIENT --query="SELECT 1;"
$CLICKHOUSE_CLIENT --query="SELECT 1; "
$CLICKHOUSE_CLIENT --query="SELECT 1 ; "

$CLICKHOUSE_CLIENT --query="SELECT 1; S" 2>&1 | grep -o 'Syntax error'
$CLICKHOUSE_CLIENT --query="SELECT 1; SELECT 2" 2>&1 | grep -o 'Syntax error'
$CLICKHOUSE_CLIENT --query="SELECT 1; SELECT 2;" 2>&1 | grep -o 'Syntax error'
$CLICKHOUSE_CLIENT --query="SELECT 1; SELECT 2; SELECT" 2>&1 | grep -o 'Syntax error'

$CLICKHOUSE_CLIENT -n --query="SELECT 1; S" 2>&1 | grep -o 'Syntax error'
$CLICKHOUSE_CLIENT -n --query="SELECT 1; SELECT 2"
$CLICKHOUSE_CLIENT -n --query="SELECT 1; SELECT 2;"
$CLICKHOUSE_CLIENT -n --query="SELECT 1; SELECT 2; SELECT" 2>&1 | grep -o 'Syntax error'

$CLICKHOUSE_CLIENT -n --query="DROP TABLE IF EXISTS t; CREATE TABLE t (x UInt64) ENGINE = TinyLog;"

$CLICKHOUSE_CLIENT --query="INSERT INTO t VALUES (1),(2),(3);"
$CLICKHOUSE_CLIENT --query="SELECT * FROM t"
$CLICKHOUSE_CLIENT --query="INSERT INTO t VALUES" <<< "(4),(5),(6)"
$CLICKHOUSE_CLIENT --query="SELECT * FROM t"

$CLICKHOUSE_CLIENT -n --query="INSERT INTO t VALUES (1),(2),(3);"
$CLICKHOUSE_CLIENT -n --query="SELECT * FROM t"
$CLICKHOUSE_CLIENT -n --query="INSERT INTO t VALUES" <<< "(4),(5),(6)"
$CLICKHOUSE_CLIENT -n --query="SELECT * FROM t"

${CLICKHOUSE_CURL} -sS "${CLICKHOUSE_URL_PARAMS}" -d "SELECT 1"
${CLICKHOUSE_CURL} -sS "${CLICKHOUSE_URL_PARAMS}" -d "SELECT 1;"
${CLICKHOUSE_CURL} -sS "${CLICKHOUSE_URL_PARAMS}" -d "SELECT 1; "
${CLICKHOUSE_CURL} -sS "${CLICKHOUSE_URL_PARAMS}" -d "SELECT 1 ; "

${CLICKHOUSE_CURL} -sS "${CLICKHOUSE_URL_PARAMS}" -d "SELECT 1; S" 2>&1 | grep -o 'Syntax error'
${CLICKHOUSE_CURL} -sS "${CLICKHOUSE_URL_PARAMS}" -d "SELECT 1; SELECT 2" 2>&1 | grep -o 'Syntax error'
${CLICKHOUSE_CURL} -sS "${CLICKHOUSE_URL_PARAMS}" -d "SELECT 1; SELECT 2;" 2>&1 | grep -o 'Syntax error'
${CLICKHOUSE_CURL} -sS "${CLICKHOUSE_URL_PARAMS}" -d "SELECT 1; SELECT 2; SELECT" 2>&1 | grep -o 'Syntax error'

${CLICKHOUSE_CURL} -sS "${CLICKHOUSE_URL_PARAMS}" -d "INSERT INTO t VALUES (1),(2),(3);"
$CLICKHOUSE_CLIENT --query="SELECT * FROM t"
${CLICKHOUSE_CURL} -sS "${CLICKHOUSE_URL_PARAMS}&query=INSERT" -d "INTO t VALUES (4),(5),(6);"
$CLICKHOUSE_CLIENT --query="SELECT * FROM t"
${CLICKHOUSE_CURL} -sS "${CLICKHOUSE_URL_PARAMS}&query=INSERT+INTO+t+VALUES" -d "(7),(8),(9)"
$CLICKHOUSE_CLIENT --query="SELECT * FROM t"

$CLICKHOUSE_CLIENT -n --query="DROP TABLE t;"
