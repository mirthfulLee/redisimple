# redisimple object

Redisimple support various object, including:

- String
- Set
- Ordered Set
- List
- HashMap

Each object provides corresponding APIs. Their concrete implementation(real data structure), which is transparent to their caller, might dynamicly transform during executing requests.

So `Proxy Design Pattern` is adopted to implement those object. The Interface serves as the proxy as well. The proxy could delivery the request to concrete implementation, and take necessary action like transform data structure before/after execution.
