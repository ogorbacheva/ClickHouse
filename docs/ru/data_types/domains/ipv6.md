## IPv6

`IPv6` — это домен, базирующийся на типе данных `FixedString(16)`,  предназначенный для хранения адресов IPv6.  Он обеспечивает компактное хранение данных с удобным для человека форматом ввода-вывода, и явно отображаемым типом данных в структуре таблицы.

### Применение

```sql
CREATE TABLE hits (url String, from IPv6) ENGINE = MergeTree() ORDER BY url;

DESCRIBE TABLE hits;
```

```
┌─name─┬─type───┬─default_type─┬─default_expression─┬─comment─┬─codec_expression─┐
│ url  │ String │              │                    │         │                  │
│ from │ IPv6   │              │                    │         │                  │
└──────┴────────┴──────────────┴────────────────────┴─────────┴──────────────────┘
```

Или вы можете использовать домен `IPv6`  в качестве ключа:

```sql
CREATE TABLE hits (url String, from IPv6) ENGINE = MergeTree() ORDER BY from;
```

`IPv6` поддерживает вставку в виде строк с текстовым представлением IPv6 адреса:

```sql
INSERT INTO hits (url, from) VALUES ('https://wikipedia.org', '2a02:aa08:e000:3100::2')('https://clickhouse.yandex', '2001:44c8:129:2632:33:0:252:2')('https://clickhouse.yandex/docs/en/', '2a02:e980:1e::1');

SELECT * FROM hits;
```

```
┌─url────────────────────────────────┬─from──────────────────────────┐
│ https://clickhouse.yandex          │ 2001:44c8:129:2632:33:0:252:2 │
│ https://clickhouse.yandex/docs/en/ │ 2a02:e980:1e::1               │
│ https://wikipedia.org              │ 2a02:aa08:e000:3100::2        │
└────────────────────────────────────┴───────────────────────────────┘
```

Значения хранятся в компактной бинарной форме:

```sql
SELECT toTypeName(from), hex(from) FROM hits LIMIT 1;
```

```
┌─toTypeName(from)─┬─hex(from)────────────────────────┐
│ IPv6             │ 200144C8012926320033000002520002 │
└──────────────────┴──────────────────────────────────┘
```

Значения с доменным типом данных не преобразуются неявно в другие типы данных, кроме `FixedString(16)`.
Если необходимо преобразовать значение типа `IPv6` в строку, то это необходимо делать явно с помощью функции `IPv6NumToString()`:

```sql
SELECT toTypeName(s), IPv6NumToString(from) AS s FROM hits LIMIT 1;
```

```
┌─toTypeName(IPv6NumToString(from))─┬─s─────────────────────────────┐
│ String                            │ 2001:44c8:129:2632:33:0:252:2 │
└───────────────────────────────────┴───────────────────────────────┘
```

Или приводить к типу данных `FixedString(16)`:

```sql
SELECT toTypeName(i), CAST(from AS FixedString(16)) AS i FROM hits LIMIT 1;
```

```
┌─toTypeName(CAST(from, 'FixedString(16)'))─┬─i───────┐
│ FixedString(16)                           │  ��� │
└───────────────────────────────────────────┴─────────┘
```

[Оригинальная статья](https://clickhouse.yandex/docs/ru/data_types/domains/ipv6) <!--hide-->
