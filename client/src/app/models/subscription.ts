import { Subscriber } from "@classes/subscriber/subscriber";

export interface Subscription {
    event: string;
    subscriber: Subscriber;
}