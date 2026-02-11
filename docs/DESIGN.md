# Mission Data Service
# SDD

## 1. Design Overview

The Mission Data Service is a lightweight REST-based microservice
implemented in C++17 and containerized using Docker.

The architecture separates transport, validation, and storage concerns.

---

## 2. Architectural Diagram

                +----------------------+
                |     HTTP Server      |
                |   (cpp-httplib)      |
                +----------+-----------+
                           |
                           v
                +----------------------+
                |   Request Handler    |
                +----------+-----------+
                           |
                           v
                +----------------------+
                |     Validator        |
                +----------+-----------+
                           |
                           v
                +----------------------+
                |    Data Store        |
                |  (Thread-Safe)       |
                +----------------------+

---

## 3. Component Descriptions

### 3.1 HTTP Server

- Library: cpp-httplib
- Responsibilities:
  - Endpoint registration
  - JSON parsing
  - HTTP status management

The HTTP layer does not perform validation beyond syntactic parsing.

---

### 3.2 Validator

Responsibilities:
- Schema enforcement
- Value constraints
- Timestamp verification

The validator is stateless and pure-functional to enable unit testing.

---

### 3.3 Data Store

Responsibilities:
- Thread-safe record storage
- Statistical computation
- Data aggregation

Synchronization Strategy:
- std::mutex protecting internal vector
- Lock scope minimized to prevent contention

---

## 4. Error Handling Strategy

- Malformed JSON → HTTP 400
- Validation failure → HTTP 422
- Empty dataset on stats → HTTP 204
- Internal failure → HTTP 500

Errors return structured JSON messages.

---

## 5. Threading Model

- Single-threaded HTTP handling (MVP scope)
- Thread-safe data store to allow future multi-threading

---

## 6. Security Considerations

- Non-root container execution
- Input validation on all external data
- No dynamic memory exposure
- RAII principles applied

---

## 7. Limitations

- No persistent storage
- No authentication
- No encryption
- No horizontal scaling

---

## 8. Design Rationale

The system intentionally uses:
- Minimal external dependencies
- Clear module boundaries
- Explicit error behavior

This mirrors common mission system design patterns emphasizing
predictability, modularity, and traceability.
