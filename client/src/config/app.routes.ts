import { Route, Routes } from '@angular/router';
import { AuthenticationPage } from '@components/authentication/authentication'
import { FeedPage } from '@components/feed/feed';
import { NotFoundPage } from '@components/not-found/not-found';
import { RecommendationPage } from '@components/recommendation/recommendation';
import { ResumePage } from '@components/resume/resume';
import { SubscriptionPage } from '@components/subscriptions/subscription';
import { authGuard } from '@guards/auth/auth-guard';
import { nonAuthGuard } from '@guards/non-auth/non-auth-guard';


interface NavigationRoute extends Route { label?: string;
                                          icon?: string }

export const routes: NavigationRoute[] = [{ path: "",               component: AuthenticationPage, canActivate: [nonAuthGuard] },
                                          { path: "feed",           component: FeedPage,           canActivate: [authGuard], label: "Jobs",            icon: "work_outline" },
                                          { path: "recommendation", component: RecommendationPage, canActivate: [authGuard], label: "Recommendations", icon: "insights" },
                                          { path: "subscription",   component: SubscriptionPage,   canActivate: [authGuard], label: "Subscriptions",   icon: "rss_feed" },
                                          { path: "resume",         component: ResumePage,         canActivate: [authGuard], label: "Resumes",         icon: "description" },
                                          { path: "**",             component: NotFoundPage }];
