# ТЕХНИЧЕСКОЕ ЗАДАНИЕ НА BACKEND (СВОДНОЕ, версия 2)

## 1. Общие сведения и причина разработки

Система создаётся для автоматизации процесса обкатки дизельных двигателей на испытательном стенде.  
**Причина разработки:** необходимость управления режимами испытаний, контроля критических параметров, снижение влияния человеческого фактора, повышение наглядности и безопасности.  
Реализуется в учебных целях с использованием моделируемых датчиков и исполнительных механизмов.

Бэкенд выполняется как **библиотека**, работающая в отдельном потоке (`QThread`), и предоставляет API фронтенду исключительно через сигналы и слоты Qt.  
Хранилище данных — CSV-файлы.

## 2. Архитектура

Бэкенд состоит из следующих модулей (каждый — `QObject`, живущий в рабочем потоке):

- **Модуль связи с фронтом (BackendWorker)** – единственная точка входа для фронтенда.
- **Модуль управления процессом (StateMachine)** – конечный автомат этапов.
- **Модуль обработки данных (DataProcessor)** – проверка критических границ, генерация предупреждений и аварий.
- **Модуль связи с моделью (ModbusDataBridge)** – циклический опрос датчиков по Modbus TCP, запись управляющих сигналов.
- **Модуль хранения данных (DataStore)** – запись в CSV, чтение для отчётов.

Обмен данными внутри потока – прямые вызовы методов; между потоком бэкенда и потоком фронтенда – сигналы/слоты Qt (`Qt::QueuedConnection`).  
Все структуры данных зарегистрированы как метатипы Qt и передаются по значению через сигналы/слоты.

## 3. Модули и их интерфейсы

### 3.1. Модуль связи с фронтом (BackendWorker)

Живёт в рабочем потоке. Владеет экземплярами всех остальных модулей, соединяет их сигналы и слоты.

**Слоты (команды от фронтенда):**

| Слот | Описание |
|------|----------|
| `void onStart(Config config)` | Запуск нового испытания. |
| `void onStop()` | Экстренная остановка. |
| `void onNextStage()` | Принудительный переход к следующему этапу. |
| `void onGetStatus()` | Запрос текущего состояния. |
| `void onGetReport(quint64 runId)` | Запрос отчёта по завершённому испытанию. |
| `void onGetHistory()` | Запрос списка завершённых испытаний. |

*(Большинство названий слотов предложено автором, кроме `onStop` и `onNextStage`, заложенных в ТЗ управления процессом.)*

**Сигналы (данные и события для фронтенда):**

| Сигнал | Описание |
|--------|----------|
| `void statusUpdated(QString state, SensorFrame lastData)` | Периодическая отправка состояния (не реже 1 раза в сек). |
| `void liveData(SensorFrame frame)` | Каждое новое измерение. |
| `void alarmEvent(AlarmEvent event)` | Немедленное аварийное уведомление. |
| `void warningEvent(WarningEvent event)` | Предупреждение. |
| `void processFinished(Report report)` | Завершение испытания, передача отчёта. |
| `void historyReady(QVector<quint64> runIds)` | Ответ на запрос истории. |

*(`statusUpdated`, `liveData`, `alarmEvent`, `warningEvent` — из исходных требований «передача нештатных сообщений»; `processFinished`, `historyReady` — предложено автором.)*

### 3.2. Модуль управления процессом (StateMachine)

Хранит текущий этап, длительности, обрабатывает переходы. Не обязательно отдельный QObject, но для удобства может иметь сигнал смены этапа.

**Слоты (вызываются BackendWorker'ом):**
- `void requestStart(Config config)` – войти в первый этап.
- `void requestNextStage()` – перейти к следующему этапу.
- `void requestAbort(QString reason)` – немедленно завершить испытание.

**Сигналы (предложено автором):**
- `void stageChanged(int oldStage, int newStage)` – для логирования и обновления UI.

**Состояния:**
`IDLE`, `COLD_CRANKING`, `START_AND_WARMUP`, `HOT_NO_LOAD`, `HOT_WITH_LOAD`, `COMPLETED`, `ABORTED`.

### 3.3. Модуль обработки данных (DataProcessor)

Принимает сырые измерения, сверяет с критическими порогами, формирует решение.

**Слоты:**
- `void processFrame(SensorFrame frame)` *(из ТЗ обработки данных – приём текущего пакета)*.

**Сигналы:**
- `void decisionReady(Decision decision)` *(из ТЗ обработки данных)*.
- `void warningRaised(WarningEvent event)` *(из ТЗ обработки данных)*.
- `void alarmRaised(AlarmEvent event)` *(из ТЗ обработки данных)*.
- `void controlNeeded(ActuatorCommand correction)` *(предложено автором)* – если требуется корректирующее воздействие.

### 3.4. Модуль связи с моделью (ModbusDataBridge)

Реализует опрос Modbus TCP и запись команд. Владеет `QModbusTcpClient`.

**Слоты:**
- `void onStartPolling()` – запуск таймера опроса.
- `void onStopPolling()` – остановка таймера.
- `void onWriteCommand(ActuatorCommand cmd)` – запись управляющих регистров в модель *(из ТЗ ModelClient)*.

**Сигналы:**
- `void dataReady(SensorFrame frame)` – после успешного чтения регистров *(из ТЗ ModelClient)*.
- `void errorOccurred(QString errorMessage)` – при критическом сбое связи *(из ТЗ ModelClient)*.

### 3.5. Модуль хранения данных (DataStore)

Работает с CSV-файлами. Не имеет сигналов, только прямые методы и слоты для записи по сигналу.

**Слоты (предложено автором):**
- `void writeRecord(MeasurementRecord record)` – дописать строку в CSV текущего испытания.
- `void writeEvent(EventRecord event)` – записать событие в журнал.

**Прямые методы (вызываются из BackendWorker):**
- `QVector<MeasurementRecord> readRecords(quint64 runId)`
- `QVector<EventRecord> readEvents(quint64 runId)`
- `Report buildReport(quint64 runId)`

**Форматы файлов (для одного испытания `runId`):**
- `reports/run_<runId>.csv` – строки MeasurementRecord с заголовком.
- `reports/events_<runId>.csv` – строки EventRecord с заголовком.

## 4. Структуры данных

Все структуры имеют открытые поля и зарегистрированы как метатипы Qt.

### Config (предложено автором)
```cpp
struct Config {
    QString modbusHost = "127.0.0.1";
    int modbusPort = 502;
    int pollIntervalMs = 1000;

    QVector<int> stageDurationSec;
    double targetRpmCold;
    double throttleWarmup;
    double throttleHotNoLoad;
    double throttleHotLoad;
    double brakeTorqueHotLoad;

    double maxRpmCold, maxRpmHot;
    double maxMotorTemp, maxDieselTemp, maxResistorTemp;
    double minDieselPressure, maxDieselPressure;
};
```

### SensorFrame (из ТЗ ModelClient)
```cpp
struct SensorFrame {
    qint64 runId;
    int stage;
    qint64 timestampMs;
    double rpm, torque;
    double dieselTemp, motorTemp, resistorTemp, dieselPressure;
    double throttle, brakeTorque;
};
```

### ActuatorCommand (из ТЗ ModelClient)
```cpp
struct ActuatorCommand {
    bool motorEnabled;
    double targetRpm;
    double throttlePosition;
    double brakeTorque;
};
```

### Decision (из ТЗ обработки данных)
```cpp
enum class Status { OK, WARNING, ALARM };

struct Decision {
    Status status;
    QString message;
    ActuatorCommand correction;   // может быть пустым
};
```

### WarningEvent (из ТЗ обработки данных)
```cpp
struct WarningEvent {
    qint64 runId, timestampMs;
    int stage;
    QString parameter;
    double currentValue, threshold;
    QString description;
};
```

### AlarmEvent (из ТЗ обработки данных)
```cpp
struct AlarmEvent {
    qint64 runId, timestampMs;
    int stage;
    QString parameter;
    double currentValue, criticalThreshold;
    QString reason;
};
```

### MeasurementRecord (предложено автором)
```cpp
struct MeasurementRecord {
    qint64 runId;
    int stage;
    qint64 timestampMs;
    double rpm, torque, dieselTemp, motorTemp, resistorTemp, dieselPressure;
    double throttle, brakeTorque;
    QString flags;   // "OK", "WARNING" и т.п.
};
```

### StageSummary (предложено автором)
```cpp
struct StageSummary {
    int stageNum;
    QString startTime, endTime;
    double avgRpm, avgTorque, maxTemperature;
    QString notes;
};
```

### EventRecord (предложено автором)
```cpp
struct EventRecord {
    qint64 runId, timestampMs;
    int stage;
    QString type;     // "stage_change", "abort", "warning", "info"
    QString message;
};
```

### Report (предложено автором)
```cpp
struct Report {
    qint64 runId;
    QString startTime, endTime;
    QString finalStatus;   // "completed" или "aborted"
    QVector<StageSummary> stages;
    QVector<EventRecord> events;
};
```

## 5. Точка сборки (main)

Не является модулем. Выполняет:
- создание `QApplication`;
- загрузку начального `Config` (из файла или аргументов);
- создание объекта `BackendStarter` (или аналогичного), который создаёт `QThread`, экземпляр `BackendWorker`, перемещает его в поток, подключает сигналы `BackendWorker` к слотам фронтендного объекта;
- запуск потока.

Фронтенд далее общается только через сигналы/слоты, передавая структуры напрямую.

## 6. Схема взаимодействия (поток данных)

### 6.1. Запуск испытания
Фронтенд → `onStart(config)`.  
`BackendWorker`:
- создаёт запись в CSV (через `DataStore`);
- переводит `StateMachine` в `COLD_CRANKING`;
- формирует первичный `ActuatorCommand` и отправляет в `ModbusDataBridge::onWriteCommand`;
- запускает опрос: `ModbusDataBridge::onStartPolling()`.

### 6.2. Цикл измерений (каждый тик таймера)
1. `ModbusDataBridge` успешно считывает регистры → `dataReady(frame)`.
2. `BackendWorker` направляет `frame` в `DataProcessor::processFrame(frame)`.
3. `DataProcessor` проверяет все критические границы и испускает один из:
   - `decisionReady(decision)` – всегда,
   - `warningRaised(event)` – если есть предупреждение,
   - `alarmRaised(event)` – если есть авария,
   - `controlNeeded(correction)` – если нужна корректировка.
4. `BackendWorker` (подписан на эти сигналы):
   - при `warningRaised`: записывает событие в журнал CSV, отправляет `warningEvent` фронтенду;
   - при `alarmRaised`:
     - вызывает `StateMachine::requestAbort(reason)`,
     - вызывает `ModbusDataBridge::onStopPolling()` и аварийный `onWriteCommand(стоп)`,
     - записывает событие,
     - отправляет `alarmEvent` фронтенду;
   - при `controlNeeded`: передаёт `correction` в `ModbusDataBridge::onWriteCommand`;
   - при любом `decisionReady`: сохраняет измерение в `DataStore::writeRecord(record)` (добавляя флаги из `decision.status`), отправляет `liveData(frame)` фронтенду.
5. Если аварии нет и длительность этапа истекла (контролирует `StateMachine` через внутренний таймер или по команде `requestNextStage`), `StateMachine` переходит к следующему этапу, сигнализирует `stageChanged`. `BackendWorker` формирует новые управляющие команды и обновляет `DataStore`.

### 6.3. Завершение испытания
- При успешном завершении последнего этапа `StateMachine` переходит в `COMPLETED`.
- При аварии – в `ABORTED`.
- В обоих случаях `BackendWorker` останавливает опрос, закрывает CSV, вызывает `DataStore::buildReport(runId)`, получает `Report` и отправляет `processFinished(report)` фронтенду.

### 6.4. Запросы от фронта вне активного испытания
- `onGetStatus` → `BackendWorker` отправляет `statusUpdated` с последним сохранённым состоянием.
- `onGetReport` → вызов `DataStore::buildReport`, ответ через `processFinished` (можно переиспользовать сигнал, передавая `report` и идентифицируя запрос).
- `onGetHistory` → чтение списка CSV-файлов в папке `reports`, возврат `historyReady(ids)`.